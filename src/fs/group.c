/*----------------------------------------------------------------------------*\
 |	group.c								      |
 |									      |
 |	Copyright © 2002-2007, Team Brainix, original authors.		      |
 |		All rights reserved.					      |
\*----------------------------------------------------------------------------*/

/*
 | This program is free software: you can redistribute it and/or modify it under
 | the terms of the GNU General Public License as published by the Free Software
 | Foundation, either version 3 of the License, or (at your option) any later
 | version.
 |
 | This program is distributed in the hope that it will be useful, but WITHOUT
 | ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 | FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 | details.
 |
 | You should have received a copy of the GNU General Public License along with
 | this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <fs/fs.h>

/*----------------------------------------------------------------------------*\
 |				  group_find()				      |
\*----------------------------------------------------------------------------*/
void group_find(inode_t *inode_ptr, blkcnt_t *blk_ptr, size_t *offset_ptr)
{

/* Find where an inode resides on its device - its block number and offset
 * within that block. */

	super_t *super_ptr;
	unsigned long group;
	unsigned long index;
	block_t *block_ptr;
	group_t *group_ptr;
	unsigned long inodes_per_block;

	/* From the superblock, calculate the inode's block group and index
	 * within that block group. */
	super_ptr = super_get(inode_ptr->dev);
	group = (inode_ptr->ino - 1) / super_ptr->s_inodes_per_group;
	index = (inode_ptr->ino - 1) % super_ptr->s_inodes_per_group;

	/* From the group descriptor, find the first block of the inode
	 * table. */
	block_ptr = block_get(inode_ptr->dev, GROUP_BLOCK);
	group_ptr = (group_t *) block_ptr->data;
	*blk_ptr = group_ptr[group].bg_inode_table;
	block_put(block_ptr, IMPORTANT);

	/* Finally, calculate the block on which an inode resides (it may or may
	 * not be the first block of the inode table) and its offset within that
	 * block. */
	inodes_per_block = super_ptr->block_size / super_ptr->s_inode_size;
	*blk_ptr += index / inodes_per_block;
	*offset_ptr = (index % inodes_per_block) * super_ptr->s_inode_size;
}
