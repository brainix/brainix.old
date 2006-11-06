/*----------------------------------------------------------------------------*\
 |	dir.c								      |
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

#include <fs/fs.h>

/*----------------------------------------------------------------------------*\
 |				    change()				      |
\*----------------------------------------------------------------------------*/
int change(const char *path, inode_t **dir)
{

/* Perform various checks, and change the specified (root or working) directory
 * to the specified path.  Return 0 on success, or a negative error number. */

	inode_t *inode_ptr = path_to_inode(path);

	if (inode_ptr == NULL)
	{
		/* The path name could not be resolved. */
		inode_put(inode_ptr);
		return -err_code;
	}
	if (!is_dir(inode_ptr))
	{
		/* The path name does not point to a directory. */
		inode_put(inode_ptr);
		return -(err_code = ENOTDIR);
	}
	if (!perm(inode_ptr, X_OK, false))
	{
		/* No search permission. */
		inode_put(inode_ptr);
		return -(err_code = EACCES);
	}

	/* All checks passed.  Change the directory. */

	inode_put(*dir);
	*dir = inode_ptr;
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_chroot()				      |
\*----------------------------------------------------------------------------*/
int do_fs_chroot(const char *path)
{

/* Change the root directory.  Return 0 on success, or a negative error
 * number. */

	if (!super_user)
		/* No chroot permission. */
		return -(err_code = EPERM);

	return change(path, &fs_proc[msg->from].root_dir);
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_chdir()				      |
\*----------------------------------------------------------------------------*/
int do_fs_chdir(const char *path)
{

/* Change the current working directory.  Return 0 on success, or a negative
 * error number. */

	return change(path, &fs_proc[msg->from].work_dir);
}
