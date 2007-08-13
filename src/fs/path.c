/*----------------------------------------------------------------------------*\
 |	path.c								      |
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
 |				path_to_inode()				      |
\*----------------------------------------------------------------------------*/
inode_t *path_to_inode(const char *path)
{

/*
 | Perform various checks, and resolve a path name to an inode in the inode
 | table.  If successful, return a pointer to the inode.  Otherwise, return
 | NULL.
 */

	const char *path_ptr = path;
	inode_t *inode_ptr = NULL;
	dev_t dev;
	ino_t ino;
	char name[NAME_MAX + 1];
	char *name_ptr = &name[0];
	unsigned char link_count = 0;
	super_t *super_ptr;

	/*
	 | If the path name is absolute, start at the root file system's root
	 | directory.  If the path name is relative, start at the process'
	 | current working directory.
	 */
	if (*path_ptr == '\0' || strlen(path_ptr) + 1 >= PATH_MAX)
	{
		/* The path name is empty or too long. */
		inode_put(inode_ptr);
		err_code = *path_ptr == '\0' ? ENOENT : ENAMETOOLONG;
		return NULL;
	}
	if (*path_ptr == '/')
	{
		/* The path name is absolute. */
		inode_ptr = fs_proc[msg->from].root_dir;
		while (*path_ptr == '/')
			path_ptr++;
	}
	else
		/* The path name is relative. */
		inode_ptr = fs_proc[msg->from].work_dir;
	if (inode_ptr->i_links_count == 0)
	{
		/* The starting directory no longer exists. */
		inode_put(inode_ptr);
		err_code = ENOENT;
		return NULL;
	}
	inode_ptr->count++;
	dev = inode_ptr->dev;
	ino = inode_ptr->ino;

	/*
	 | Iterate over each path name component.  For example, for the path
	 | name "/home/rajiv/porn.jpg", the components would be "home", "rajiv",
	 | and "porn.jpg".
	 */
	while (!err_code && *path_ptr != '\0')
	{
		/* From the path name, extract the next component name. */
		while (*path_ptr != '/' && *path_ptr != '\0')
		{
			*name_ptr++ = *path_ptr++;
			if (name_ptr == &name[NAME_MAX])
			{
				/* The next component name is too long. */
				inode_put(inode_ptr);
				err_code = ENAMETOOLONG;
				return NULL;
			}
		}
		*name_ptr = '\0';

		/*
		 | Find the next component within the current component.  Upon
		 | completing this step, the next component becomes the current
		 | component.
		 */
		if (!perm(inode_ptr, X_OK, false))
		{
			/* No search permission in the current component. */
			inode_put(inode_ptr);
			err_code = EACCES;
			return NULL;
		}
		ino = search_dir(inode_ptr, name);
		if (ino == 0)
		{
			/*
			 | The next component could not be found in the current
			 | component.
			 */
			inode_put(inode_ptr);
			err_code = ENOENT;
			return NULL;
		}
		inode_put(inode_ptr);
		inode_ptr = inode_get(dev, ino);

		/*
		 | If the current component is a symbolic link, jump to wherever
		 | it points.
		 */
		if (inode_ptr->i_mode & EXT2_S_IFLNK)
		{
			if (++link_count >= SYMLOOP_MAX)
			{
				inode_put(inode_ptr);
				err_code = ELOOP;
				return NULL;
			}
			/* TODO: Jump to wherever it points. */
		}

		/*
		 | If a file system is mounted on the current component, jump to
		 | the root directory of the mounted file system.
		 */
		if (inode_ptr->mounted)
		{
			super_ptr = &super[0];
			while (super_ptr->mount_point_inode_ptr != inode_ptr)
				super_ptr++;
			dev = super_ptr->dev;
			ino = EXT2_ROOT_INO;
			inode_put(inode_ptr);
			inode_ptr = inode_get(dev, ino);
		}

		/* Skip past the slashes trailing the current component name. */
		if (*path_ptr == '/' && !is_dir(inode_ptr))
		{
			/*
			 | A slash trails the current component name, but the
			 | current component is not a directory.
			 */
			inode_put(inode_ptr);
			err_code = ENOTDIR;
			return NULL;
		}
		while (*path_ptr == '/')
			path_ptr++;
	}

	return inode_ptr;
}
