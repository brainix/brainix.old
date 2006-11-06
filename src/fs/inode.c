/*----------------------------------------------------------------------------*\
 |	inode.c								      |
 |									      |
 |	Copyright © 2002-2006, Team Brainix, original authors.		      |
 |		All rights reserved.					      |
\*----------------------------------------------------------------------------*/

/*
 | This program is Free Software; you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation; either version 2 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program; if not, write to:
 |
 |	The Free Software Foundation, Inc.
 |	59 Temple Place, Suite 330
 |	Boston MA 02111-1307
 */

#include <fs/fs.h>

/*----------------------------------------------------------------------------*\
 |				  inode_init()				      |
\*----------------------------------------------------------------------------*/
void inode_init(void)
{

/* Initialize the inode table. */

	inode_t *inode_ptr;

	/* Initialize each slot in the table. */
	for (inode_ptr = &inode[0]; inode_ptr < &inode[NUM_INODES]; inode_ptr++)
	{
		inode_ptr->dev = NO_DEV;
		inode_ptr->ino = 0;
		inode_ptr->count = 0;
		inode_ptr->mounted = false;
		inode_ptr->dirty = false;
	}
}

/*----------------------------------------------------------------------------*\
 |				   inode_rw()				      |
\*----------------------------------------------------------------------------*/
void inode_rw(inode_t *inode_ptr, bool read)
{

/* If read is true, read an inode from its block into the inode table.
 * Otherwise, write an inode from the table to its block. */

	blkcnt_t blk;
	size_t offset;
	block_t *block_ptr;
	super_t *super_ptr = super_get(inode_ptr->dev);

	if (!inode_ptr->dirty)
		/* The inode in the table is already synchronized with the inode
		 * on its block.  No reason to read or write anything. */
		return;

	/* Get the block on which the inode resides. */
	group_find(inode_ptr, &blk, &offset);
	block_ptr = block_get(inode_ptr->dev, blk);

	if (read)
		/* Read the inode from its block into the table. */
		memcpy(inode_ptr, &block_ptr->data[offset],
			super_ptr->s_inode_size);
	else
	{
		/* Write the inode from the table to its block, and mark the
		 * block dirty. */
		memcpy(&block_ptr->data[offset], inode_ptr,
			super_ptr->s_inode_size);
		block_ptr->dirty = true;
	}

	/* Put the block on which the inode resides, and mark the inode in the
	 * table as no longer dirty. */
	block_put(block_ptr, IMPORTANT);
	inode_ptr->dirty = false;
}

/*----------------------------------------------------------------------------*\
 |				  inode_get()				      |
\*----------------------------------------------------------------------------*/
inode_t *inode_get(dev_t dev, ino_t ino)
{

/* Search the inode table for an inode.  If it is found, return a pointer to it.
 * Otherwise, read the inode into the table, and return a pointer to it. */

	inode_t *inode_ptr;

	/* Search the table for the inode. */
	for (inode_ptr = &inode[0]; inode_ptr < &inode[NUM_INODES]; inode_ptr++)
		if (inode_ptr->dev == dev && inode_ptr->ino == ino)
		{
			/* Found the inode.  Increment the number of times it is
			 * used, and return a pointer to it. */
			inode_ptr->count++;
			return inode_ptr;
		}

	/* The inode is not in the table.  Find a free slot. */
	for (inode_ptr = &inode[0]; inode_ptr < &inode[NUM_INODES]; inode_ptr++)
		if (inode_ptr->count == 0)
		{
			/* Found a free slot.  Read the inode into it, and
			 * return a pointer into it. */
			inode_ptr->dev = dev;
			inode_ptr->ino = ino;
			inode_ptr->count = 1;
			inode_ptr->mounted = false;
			inode_ptr->dirty = true;
			inode_rw(inode_ptr, READ);
			return inode_ptr;
		}

	/* There are no free slots in the table.  Vomit. */
	panic("inode_get", "no free inodes");
	return NULL;
}

/*----------------------------------------------------------------------------*\
 |				  inode_put()				      |
\*----------------------------------------------------------------------------*/
void inode_put(inode_t *inode_ptr)
{

/* Decrement the number of times an inode is used.  If no one is using it,
 * update its access time (if necessary), and write it to its block. */

	if (inode_ptr == NULL || --inode_ptr->count > 0)
		return;
	if ((inode_ptr->i_flags & EXT2_NOATIME_FL) != EXT2_NOATIME_FL)
	{
		inode_ptr->i_atime = do_time(NULL);
		inode_ptr->dirty = true;
	}
	inode_rw(inode_ptr, WRITE);
}

/*----------------------------------------------------------------------------*\
 |				  block_find()				      |
\*----------------------------------------------------------------------------*/
blkcnt_t block_find(inode_t *inode_ptr, blkcnt_t n)
{

/* Return an inode's nth (data) block number. */

	dev_t dev = inode_ptr->dev;
	super_t *super_ptr = super_get(dev);
	blksize_t size = super_ptr->block_size;
	blkcnt_t blk = 0, div = 1, *blk_ptr;
	block_t *block_ptr;

	/* Does the inode directly contain the nth block number? */
	if (IS_DIRECT(size, n))
		/* Yes - return. */
		return inode_ptr->i_block[n];

	/* Figure out whether the nth block number must be found via the
	 * indirect, bi-indirect, or tri-indirect block. */
	if (IS_INDIRECT(size, n))
		blk = inode_ptr->i_block[12];
	if (IS_BI_INDIRECT(size, n))
		blk = inode_ptr->i_block[13];
	if (IS_TRI_INDIRECT(size, n))
		blk = inode_ptr->i_block[14];

	/* Prepare for iteration. */
	n -= NUM_DIRECT(size);
	for (; div * NUM_INDIRECT(size) <= n; div *= NUM_INDIRECT(size))
		;

	/* Iterate. */
	for (; ; n %= div, div /= NUM_INDIRECT(size))
	{
		/* At the beginning of the iteration, if blk is tri-indirect, at
		 * the end, it'll be bi-indirect.  If it's bi-indirect, it'll be
		 * indirect.  If it's indirect, it'll be direct. */
		block_ptr = block_get(dev, blk);
		blk_ptr = (blkcnt_t *) block_ptr->data;
		blk = blk_ptr[n / div];
		block_put(block_ptr, IMPORTANT);
		if (n < NUM_INDIRECT(size))
			/* blk is now the inode's nth block number.  Return. */
			return blk;
	}
}

/*----------------------------------------------------------------------------*\
 |				    is_blk()				      |
\*----------------------------------------------------------------------------*/
bool is_blk(inode_t *inode_ptr)
{

/* Is an inode a block device? */

	if (inode_ptr == NULL)
		return false;
	return (inode_ptr->i_mode & EXT2_S_IFMT) == EXT2_S_IFBLK;
}

/*----------------------------------------------------------------------------*\
 |				    is_dir()				      |
\*----------------------------------------------------------------------------*/
bool is_dir(inode_t *inode_ptr)
{

/* Is an inode a directory? */

	if (inode_ptr == NULL)
		return false;
	return (inode_ptr->i_mode & EXT2_S_IFMT) == EXT2_S_IFDIR;
}

/*----------------------------------------------------------------------------*\
 |				    is_chr()				      |
\*----------------------------------------------------------------------------*/
bool is_chr(inode_t *inode_ptr)
{

/* Is an inode a character device? */

	if (inode_ptr == NULL)
		return false;
	return (inode_ptr->i_mode & EXT2_S_IFMT) == EXT2_S_IFCHR;
}
