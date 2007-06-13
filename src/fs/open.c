/*----------------------------------------------------------------------------*\
 |	open.c								      |
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
 |				  do_fs_open()				      |
\*----------------------------------------------------------------------------*/
int do_fs_open(const char *path, int oflag, mode_t mode)
{
 
	debug(1-FS_ESOTERIC,"FileSystem.Open(): Attempting to open %s\n",path);
 
	int fildes = 0;
	inode_t *inode_ptr = NULL;
	dev_t dev;
	bool block;

	if (!err_code)
	{
		fildes = descr_get();
 
	debug(1-FS_ESOTERIC,"FileSystem.Open(): ERROR WITH OPENING THE DEVICE! %d\n",fildes);
 
	}
	if (!err_code)
	{
		inode_ptr = path_to_inode(path);
 
	debug(1-FS_ESOTERIC,"FileSystem.Open(): No Error opening the device \n");
 

	}
	if (err_code)
	{
 
	debug(1-FS_ESOTERIC,"FileSystem.Open(): Cheap ass solution in place. \n");
 
		descr_put(fildes);
		inode_put(inode_ptr);
		return -err_code;
	}

	if (is_blk(inode_ptr) || is_chr(inode_ptr))
	{
		dev = inode_to_dev(inode_ptr);
		block = is_blk(inode_ptr);
		dev_open_close(dev, block, OPEN);
		if (err_code)
		{
			descr_put(fildes);
			inode_put(inode_ptr);
			return -err_code;
		}
	}

	fs_proc[msg->from].open_descr[fildes]->inode_ptr = inode_ptr;
	fs_proc[msg->from].open_descr[fildes]->count = 1;
	fs_proc[msg->from].open_descr[fildes]->offset = 0;
	fs_proc[msg->from].open_descr[fildes]->status = 0;
	fs_proc[msg->from].open_descr[fildes]->mode = mode;
	return fildes;
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_close()				      |
\*----------------------------------------------------------------------------*/
int do_fs_close(int fildes)
{
	inode_t *inode_ptr;
	dev_t dev;
	bool block;

	if (fs_proc[msg->from].open_descr[fildes] == NULL)
		return -(err_code = EBADF);

	inode_ptr = fs_proc[msg->from].open_descr[fildes]->inode_ptr;
	if (fs_proc[msg->from].open_descr[fildes]->count == 1)
	{
		if (is_blk(inode_ptr) || is_chr(inode_ptr))
		{
			dev = inode_to_dev(inode_ptr);
			block = is_blk(inode_ptr);
			dev_open_close(dev, block, CLOSE);
		}
		fs_proc[msg->from].open_descr[fildes]->inode_ptr = NULL;
		fs_proc[msg->from].open_descr[fildes]->count = 0;
		fs_proc[msg->from].open_descr[fildes]->offset = 0;
		fs_proc[msg->from].open_descr[fildes]->status = 0;
		fs_proc[msg->from].open_descr[fildes]->mode = 0;
	}
	descr_put(fildes);
	inode_put(inode_ptr);
	return 0;
}
