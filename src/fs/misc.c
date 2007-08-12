/*----------------------------------------------------------------------------*\
 |	misc.c								      |
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
 |				  do_fs_sync()				      |
\*----------------------------------------------------------------------------*/
void do_fs_sync(void)
{

/* Write all cached data to the devices. */

	super_t *super_ptr;
	inode_t *inode_ptr;
	block_t *block_ptr;

	/* Write the superblocks to the block cache. */
	for (super_ptr = &super[0]; super_ptr < &super[NUM_SUPERS]; super_ptr++)
		super_write(super_ptr);

	/* Write the inodes to the block cache. */
	for (inode_ptr = &inode[0]; inode_ptr < &inode[NUM_INODES]; inode_ptr++)
		inode_rw(inode_ptr, WRITE);

	/* Write the cached blocks to their devices.  This must be done last. */
	for (block_ptr = &block[0]; block_ptr < &block[NUM_BLOCKS]; block_ptr++)
		block_rw(block_ptr, WRITE);
}
