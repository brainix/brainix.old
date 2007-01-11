/*----------------------------------------------------------------------------*\
 |	link.c								      |
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
 |				  do_fs_link()				      |
\*----------------------------------------------------------------------------*/
int do_fs_link(const char *path1, const char *path2)
{

/* Perform various checks, and create a new link (directory entry) for an
 * existing file.  Return 0 on success, or a negative error number. */

	inode_t *inode_ptr_1 = path_to_inode(path1);
	inode_t *inode_ptr_2 = path_to_inode(path2);

	if (inode_ptr_1 == NULL)
	{
		/* The first path name could not be resolved. */
		inode_put(inode_ptr_1);
		inode_put(inode_ptr_2);
		return -err_code;
	}
	if (inode_ptr_1->i_links_count >= LINK_MAX)
	{
		/* The file is already linked the maximum number of times. */
		inode_put(inode_ptr_1);
		inode_put(inode_ptr_2);
		return -(err_code = EMLINK);
	}
	if (!super_user && is_dir(inode_ptr_1))
	{
		/* Only the super user may link to directories. */
		inode_put(inode_ptr_1);
		inode_put(inode_ptr_2);
		return -(err_code = EPERM);
	}
	if (inode_ptr_2 != NULL)
	{
		/* The second path name resolved. */
		inode_put(inode_ptr_1);
		inode_put(inode_ptr_2);
		return -(err_code = EEXIST);
	}

	/* All checks passed.  Perform the link. */

	inode_ptr_1->i_links_count++;
	inode_ptr_1->dirty = true;
	inode_put(inode_ptr_1);
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_unlink()				      |
\*----------------------------------------------------------------------------*/
int do_fs_unlink(const char *path)
{

/* Perform various checks, and remove a link to a file.  Return 0 on success, or
 * a negative error number. */

	inode_t *inode_ptr = path_to_inode(path);

	if (inode_ptr == NULL)
	{
		/* The path name could not be resolved. */
		inode_put(inode_ptr);
		return -err_code;
	}
	if (inode_ptr->ino == EXT2_ROOT_INO)
	{
		inode_put(inode_ptr);
		return -(err_code = EBUSY);
	}
	if (!super_user && is_dir(inode_ptr))
	{
		/* Only the super user may unlink directories. */
		inode_put(inode_ptr);
		return -(err_code = EPERM);
	}

	/* All checks passed.  Perform the unlink. */

	if (--inode_ptr->i_links_count == 0)
		;
	inode_ptr->dirty = true;
	inode_put(inode_ptr);
	return 0;
}
