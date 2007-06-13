/*----------------------------------------------------------------------------*\
 |	read.c								      |
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
 |				  rw_fildes()				      |
\*----------------------------------------------------------------------------*/
ssize_t rw_fildes(bool read, int fildes, void *buf, size_t nbyte)
{
	debug(1-FS_ESOTERIC, "read.rw_fildes(): Attempting to read/write fildes.\n");
	file_ptr_t *open_descr = fildes_to_open_descr(fildes);
	inode_t *inode_ptr = fildes_to_inode(fildes);
	off_t off = open_descr == NULL ? 0 : open_descr->offset;
	ssize_t completed = 0, remaining = nbyte, size;
	dev_t dev = inode_to_dev(inode_ptr);
	blkcnt_t inode_block, inode_index, dev_block;
	block_t *block_ptr;
	char *p1, *p2;
	super_t *super_ptr;

	if (open_descr == NULL)
	{
		debug(2-FS_ESOTERIC, "read.rw_fildes(): open descriptor null.\n");
		return -(err_code = EBADF);
	}
	if ((open_descr->mode & (O_RDWR | read ? O_RDONLY : O_WRONLY)) == 0)
	{
		debug(2-FS_ESOTERIC, "read.rw_fildes(): no permissions to read/write fildes.\n");
		return -(err_code = EPERM);
	}
	if (inode_ptr == NULL)
	{
		debug(2-FS_ESOTERIC, "read.rw_fildes(): The inode pointer is null.\n");
		return -(err_code = EBADF);
	}
	if (!perm(inode_ptr, read ? R_OK : W_OK, false))
	{
		debug(2-FS_ESOTERIC, "read.rw_fildes(): do not have permission to read/write fildes to the inode.\n");
		return -(err_code = EPERM);
	}
	if (read && is_dir(inode_ptr))
	{
		debug(2-FS_ESOTERIC, "read.rw_fildes(): error trying to write fildes to a directory.\n");
		return -(err_code = EISDIR);
	}
	if (!read && off + nbyte > inode_ptr->i_blocks * BLOCK_SIZE)
	{
		debug(2-FS_ESOTERIC, "read.rw_fildes(): error file too big (EFBIG?).\n");
		return -(err_code = EFBIG);
	}
	if (nbyte == 0)
	{
		debug(2-FS_ESOTERIC, "read.rw_fildes(): nbytes to read or write is 0.\n");
		return 0;
	}
	if (is_blk(inode_ptr) || is_chr(inode_ptr))
	{
		size = dev_rw(dev, is_blk(inode_ptr), read, off, nbyte, buf);
		if (err_code)
			return -err_code;
		remaining -= size;
		completed += size;
	}
	else
	{
		inode_block = off / BLOCK_SIZE;
		inode_index = off % BLOCK_SIZE;
		while (remaining)
		{
			dev_block = block_find(inode_ptr, inode_block++);
			block_ptr = block_get(dev, dev_block);
			p1 = read ? (char *) buf : block_ptr->data;
			p2 = read ? block_ptr->data : (char *) buf;
			size = remaining < BLOCK_SIZE ? remaining : BLOCK_SIZE;
			memcpy(p1 + completed, p2 + inode_index, size);
			inode_index = 0;
			remaining -= size;
			completed += size;
			if (!read)
				block_ptr->dirty = true;
			block_put(block_ptr, UNIMPORTANT);
		}
	}

	open_descr->offset += completed;
	if (!read)
	{
		super_ptr = super_get(inode_ptr->dev);
		super_ptr->s_wtime = inode_ptr->i_mtime = do_time(NULL);
		super_ptr->dirty = true;
	}
	debug(1-FS_ESOTERIC, "read.rw_fildes(): completed.\n");
	return completed;
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				  do_fs_read()				      |
\*----------------------------------------------------------------------------*/
ssize_t do_fs_read(int fildes, void *buf, size_t nbyte)
{
	return rw_fildes(READ, fildes, buf, nbyte);
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_write()				      |
\*----------------------------------------------------------------------------*/
ssize_t do_fs_write(int fildes, void *buf, size_t nbyte)
{
	return rw_fildes(WRITE, fildes, buf, nbyte);
}
