/*----------------------------------------------------------------------------*\
 |	block.c								      |
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
 |				  block_init()				      |
\*----------------------------------------------------------------------------*/
void block_init(void)
{

/* Initialize the block cache. */

	block_t *block_ptr;

	/* Initialize each block in the cache. */
	for (block_ptr = &block[0]; block_ptr < &block[NUM_BLOCKS]; block_ptr++)
	{
		block_ptr->dev = NO_DEV;
		block_ptr->blk = 0;
		block_ptr->count = 0;
		block_ptr->dirty = false;
		block_ptr->prev = block_ptr - 1;
		block_ptr->next = block_ptr + 1;
	}

	/* Make the cache linked list circular. */
	block[0].prev = &block[NUM_BLOCKS - 1];
	block[NUM_BLOCKS - 1].next = &block[0];

	/* Initialize the least recently used position in the cache. */
	lru = &block[0];
}

/*----------------------------------------------------------------------------*\
 |				recently_used()				      |
\*----------------------------------------------------------------------------*/
void recently_used(block_t *block_ptr, bool most)
{

/* If most is true, move a block to the most recently used position in the
 * cache.  Otherwise, move a block to the least recently used position in the
 * cache. */

	/* Remove the block from the cache linked list. */
	if (block_ptr == lru)
		lru = lru->next;
	block_ptr->prev->next = block_ptr->next;
	block_ptr->next->prev = block_ptr->prev;

	/* Reinsert the block into the desired position in the cache linked
	 * list. */
	block_ptr->prev = lru->prev;
	block_ptr->next = lru;
	block_ptr->next->prev = block_ptr->prev->next = block_ptr;
	if (!most)
		lru = lru->prev;
}

/*----------------------------------------------------------------------------*\
 |				   block_rw()				      |
\*----------------------------------------------------------------------------*/
void block_rw(block_t *block_ptr, bool read)
{

/* If read is true, read a block from its device into the cache.  Otherwise,
 * write a block from the cache to its device. */

	dev_t dev = block_ptr->dev;
	off_t off = block_ptr->blk * BLOCK_SIZE;
	void *buf = block_ptr->data;
	super_t *super_ptr;

	if (!block_ptr->dirty)
		/* The cached block is already synchronized with the block on
		 * its device.  No reason to read or write anything. */
		return;

	/* Read the block from its device into the cache, or write the block
	 * from the cache to its device. */
	dev_rw(dev, BLOCK, read, off, BLOCK_SIZE, buf);

	/* The cached block is now synchronized with the block on its device. */
	block_ptr->dirty = false;
	if (!read)
	{
		super_ptr = super_get(block_ptr->dev);
		super_ptr->s_wtime = do_time(NULL);
		super_ptr->dirty = true;
	}
}

/*----------------------------------------------------------------------------*\
 |				   dev_sync()				      |
\*----------------------------------------------------------------------------*/
void dev_sync(dev_t dev)
{

/* Write all of a device's cached blocks to the device. */

	blkcnt_t j;

	/* Iterate over each block in the cache. */
	for (j = 0; j < NUM_BLOCKS; j++)
		/* Does the current block reside on the device being synched? */
		if (block[j].dev == dev)
			/* Yes - write it. */
			block_rw(&block[j], WRITE);
}

/*----------------------------------------------------------------------------*\
 |				  dev_purge()				      |
\*----------------------------------------------------------------------------*/
void dev_purge(dev_t dev)
{

/* Reinitialize all of a device's cached blocks.  This is done on unmount so
 * that (for example) if an old floppy is unmounted and removed, and a new
 * floppy is inserted and mounted, the cached blocks for the old floppy are not
 * used when accessing the new floppy. */

	blkcnt_t j;

	/* Iterate over each block in the cache. */
	for (j = 0; j < NUM_BLOCKS; j++)
		/* Does the current block reside on the device being purged? */
		if (block[j].dev == dev)
		{
			/* Yes - reinitialize it and force it to the least
			 * recently used position, as its lifetime has ended. */
			block[j].dev = NO_DEV;
			block[j].blk = 0;
			block[j].count = 0;
			block[j].dirty = false;
			recently_used(&block[j], LEAST);
		}
}

/*----------------------------------------------------------------------------*\
 |				  block_get()				      |
\*----------------------------------------------------------------------------*/
block_t *block_get(dev_t dev, blkcnt_t blk)
{

/* Search the cache for a block.  If it is found, return a pointer to it.
 * Otherwise, evict a free block, cache the block, and return a pointer to
 * it. */

	block_t *block_ptr;

	/* Search the cache for the block. */
	for (block_ptr = lru->prev; ; )
		if (block_ptr->dev == dev && block_ptr->blk == blk)
		{
			/* Found the block.  Increment the number of times it is
			 * used, mark it recently used, and return a pointer to
			 * it. */
			block_ptr->count++;
			recently_used(block_ptr, MOST);
			return block_ptr;
		}
		else if ((block_ptr = block_ptr->prev) == lru->prev)
			/* Oops - we've searched the entire cache already. */
			break;

	/* The requested block is not cached.  Search the cache for the least
	 * recently used free block. */
	for (block_ptr = lru; ; )
		if (block_ptr->count == 0)
		{
			/* Found the least recently used free block.  Evict it.
			 * Cache the requested block, mark it recently used, and
			 * return a pointer to it. */
			block_rw(block_ptr, WRITE);
			block_ptr->dev = dev;
			block_ptr->blk = blk;
			block_ptr->count = 1;
			block_ptr->dirty = true;
			block_rw(block_ptr, READ);
			recently_used(block_ptr, MOST);
			return block_ptr;
		}
		else if ((block_ptr = block_ptr->next) == lru)
			/* Oops - we've searched the entire cache already. */
			break;

	/* There are no free blocks in the cache.  Vomit. */
	panic("block_get", "no free blocks");
	return NULL;
}

/*----------------------------------------------------------------------------*\
 |				  block_put()				      |
\*----------------------------------------------------------------------------*/
void block_put(block_t *block_ptr, bool important)
{

/* Decrement the number of times a block is used.  If no one is using it, write
 * it to its device (if necessary). */

	if (block_ptr == NULL || --block_ptr->count > 0)
		return;
	switch (ROBUST)
	{
		case PARANOID:
			block_rw(block_ptr, WRITE);
			return;
		case SANE:
			if (important)
				block_rw(block_ptr, WRITE);
			return;
		case SLOPPY:
			return;
	}
}
