/*----------------------------------------------------------------------------*\
 |	group.h								      |
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

#ifndef _FS_GROUP_H
#define _FS_GROUP_H

#include <fs/inode.h>
#include <sys/types.h>

/* A group descriptor describes a single block group: */
typedef struct
{
	unsigned long bg_block_bitmap;       /* First block of block bitmap. */
	unsigned long bg_inode_bitmap;       /* First block of inode bitmap. */
	unsigned long bg_inode_table;        /* First block of inode table.  */
	unsigned short bg_free_blocks_count; /* Number of free blocks.       */
	unsigned short bg_free_inodes_count; /* Number of free inodes.       */
	unsigned short bg_used_dirs_count;   /* Number of directories.       */
	unsigned short bg_pad;               /* Padding.                     */
	unsigned long bg_reserved[3];        /* Reserved.                    */
} group_t;

/* Function prototype: */
void group_find(inode_t *inode_ptr, blkcnt_t *blk_ptr, size_t *offset_ptr);

#endif
