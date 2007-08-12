/*----------------------------------------------------------------------------*\
 |	stat.c								      |
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
 |				fill_stat_buf()				      |
\*----------------------------------------------------------------------------*/
void fill_stat_buf(inode_t *inode_ptr, struct stat *buf)
{
	buf->st_dev = inode_ptr->dev;
	buf->st_ino = inode_ptr->ino;
	buf->st_mode = inode_ptr->i_mode;
	buf->st_nlink = inode_ptr->i_links_count;
	buf->st_uid = inode_ptr->i_uid;
	buf->st_gid = inode_ptr->i_gid;
	buf->st_size = inode_ptr->i_size;
	buf->st_atime = inode_ptr->i_atime;
	buf->st_mtime = inode_ptr->i_mtime;
	buf->st_ctime = inode_ptr->i_ctime;
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_fstat()				      |
\*----------------------------------------------------------------------------*/
int do_fs_fstat(int fildes, struct stat *buf)
{
	file_ptr_t *open_descr = fildes_to_open_descr(fildes);

	if (open_descr == NULL)
		return -(err_code = EBADF);

	fill_stat_buf(open_descr->inode_ptr, buf);
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				  do_fs_stat()				      |
\*----------------------------------------------------------------------------*/
int do_fs_stat(const char *path, struct stat *buf)
{
	inode_t *inode_ptr = path_to_inode(path);

	if (inode_ptr == NULL)
		/* The path name could not be resolved. */
		return -err_code;

	fill_stat_buf(inode_ptr, buf);
	inode_put(inode_ptr);
	return 0;
}
