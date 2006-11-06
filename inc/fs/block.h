/*----------------------------------------------------------------------------*\
 |	block.h								      |
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

#ifndef _FS_BLOCK_H
#define _FS_BLOCK_H

#include <config.h>
#include <sys/types.h>
#include <stdbool.h>

#define MOST		true
#define LEAST		false
#define IMPORTANT	true
#define UNIMPORTANT	false

/* Layout of blocks on a device: */
#define SUPER_BLOCK	1 /* Block containing superblock.        */
#define GROUP_BLOCK	2 /* Block containing group descriptors. */

/* A cached block is a copy in RAM of a block on a device: */
typedef struct block
{
	/* The following field resides on the device: */
	char data[BLOCK_SIZE]; /* Block data. */

	/* The following fields do not reside on the device: */
	dev_t dev;             /* Device the block is on.            */
	blkcnt_t blk;          /* Block number on its device.        */
	unsigned char count;   /* Number of times the block is used. */
	bool dirty;            /* Block changed since read.          */
	struct block *prev;    /* Previous block in the list.        */
	struct block *next;    /* Next block in the list.            */
} block_t;

/* Block cache: */
block_t block[NUM_BLOCKS]; /* Used to access the cache as an array. */
block_t *lru;              /* Least recently used spot in the list. */

/* Function prototypes: */
void block_init(void);
void recently_used(block_t *block_ptr, bool most);
void block_rw(block_t *block_ptr, bool read);
void dev_sync(dev_t dev);
void dev_purge(dev_t dev);
block_t *block_get(dev_t dev, blkcnt_t blk);
void block_put(block_t *block_ptr, bool important);

#endif
