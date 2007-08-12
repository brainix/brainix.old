/*----------------------------------------------------------------------------*\
 |	search.c							      |
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
 |				 search_block()				      |
\*----------------------------------------------------------------------------*/
ino_t search_block(dev_t dev, blkcnt_t blk, char *name)
{

/* Search a single directory data block for an entry.  If it is found, return
 * its inode number.  Otherwise, return 0. */

	block_t *block_ptr = block_get(dev, blk);
	dir_entry_t *dir_entry = (dir_entry_t *) block_ptr->data;
	ino_t ino = 0;

	do
	{
		if (dir_entry->name_len == strlen(name))
			if (strncmp(dir_entry->name, name, strlen(name)) == 0)
				ino = dir_entry->inode;
		dir_entry = (dir_entry_t *)
			((char *) dir_entry + dir_entry->rec_len);
	} while (ino == 0 && dir_entry->inode != 0);
	block_put(block_ptr, IMPORTANT);
	return ino;
}

/*----------------------------------------------------------------------------*\
 |				  search_dir()				      |
\*----------------------------------------------------------------------------*/
ino_t search_dir(inode_t *inode_ptr, char *name)
{

/* Search an entire directory for an entry.  If it is found, return its inode
 * number.  Otherwise, return 0. */

	blkcnt_t num_blocks, blk, j = 0;
	ino_t ino;

	num_blocks = inode_ptr->i_blocks;
	num_blocks /= (super_get(inode_ptr->dev))->block_size / 512;
	do
	{
		blk = block_find(inode_ptr, j);
		ino = search_block(inode_ptr->dev, blk, name);
	} while (ino == 0 && ++j < num_blocks);
	return ino;
}
