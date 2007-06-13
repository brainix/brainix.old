/*----------------------------------------------------------------------------*\
 |	super.c								      |
 |									      |
 |	Copyright © 2002-2007, Team Brainix, original authors.		      |
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
 |				  super_init()				      |
\*----------------------------------------------------------------------------*/
void super_init(void)
{

/* Initialize the superblock table. */

	super_t *super_ptr;

	/* Initialize each slot in the table. */
	for (super_ptr = &super[0]; super_ptr < &super[NUM_SUPERS]; super_ptr++)
	{
		super_ptr->dev = NO_DEV;
		super_ptr->block_size = 0;
		super_ptr->frag_size = 0;
		super_ptr->mount_point_inode_ptr = NULL;
		super_ptr->root_dir_inode_ptr = NULL;
		super_ptr->dirty = false;
	}
}

/*----------------------------------------------------------------------------*\
 |				  super_get()				      |
\*----------------------------------------------------------------------------*/
super_t *super_get(dev_t dev)
{

/* Search the superblock table for a superblock.  If it is found, return a
 * pointer to it.  Otherwise, return NULL. */

	super_t *super_ptr;

	/* Search the table for the superblock. */
	for (super_ptr = &super[0]; super_ptr < &super[NUM_SUPERS]; super_ptr++)
		if (super_ptr->dev == dev)
			/* Found the superblock.  Return a pointer to it. */
			return super_ptr;

	/* The superblock is not in the table. */
	return NULL;
}

/*----------------------------------------------------------------------------*\
 |				  super_read()				      |
\*----------------------------------------------------------------------------*/
super_t *super_read(dev_t dev)
{
#if DEBUG
	printf("fs.super_read(): Read a superblock from its block into the superblock table, and return a pointer to it.\n");
#endif
/* Read a superblock from its block into the superblock table, and return a
 * pointer to it. */

	super_t *super_ptr;
	block_t *block_ptr;

#if DEBUG
	printf("fs.super_read(): Find a free slot in the table.\n");
#endif
	/* Find a free slot in the table. */
	if ((super_ptr = super_get(NO_DEV)) == NULL)
	{
#if DEBUG
		printf("fs.super_read(): There are no free slots in the table -- too many mounted file systems!\n");
#endif
		/* There are no free slots in the table --- too many mounted
		 * file systems. */
		return NULL;
	}
#if DEBUG
	printf("fs.super_read(): Copy the superblock from its block into the free slot.\n");
#endif
	/* Copy the superblock from its block into the free slot. */
	block_ptr = block_get(dev, SUPER_BLOCK);
	memcpy(super_ptr, block_ptr->data, offsetof(super_t, dev));
	block_put(block_ptr, IMPORTANT);

	return super_ptr;
}

/*----------------------------------------------------------------------------*\
 |				 super_write()				      |
\*----------------------------------------------------------------------------*/
void super_write(super_t *super_ptr)
{

/* Write a superblock from the superblock table to its block. */

	block_t *block_ptr;

	if (!super_ptr->dirty)
		/* The superblock in the table is already synchronized with the
		 * superblock on its block.  No reason to write anything. */
		return;

	/* Copy the superblock from the table to its block. */
	block_ptr = block_get(super_ptr->dev, SUPER_BLOCK);
	memcpy(block_ptr->data, super_ptr, offsetof(super_t, dev));
	block_ptr->dirty = true;
	block_put(block_ptr, IMPORTANT);

	/* The superblock in the table is now synchronized with the superblock
	 * on its block. */
	super_ptr->dirty = false;
}
