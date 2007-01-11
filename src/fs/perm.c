/*----------------------------------------------------------------------------*\
 |	perm.c								      |
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
 |				     perm()				      |
\*----------------------------------------------------------------------------*/
bool perm(inode_t *inode_ptr, unsigned char requested, bool access)
{

/* Check an inode's permissions.  If the requested access is permitted, return
 * true.  Otherwise, return false. */

	gid_t gid;
	uid_t uid;
	unsigned char permitted;

	/* Garbage in, garbage out. */
	if (inode_ptr == NULL)
		return false;

	/* If called by access(), use the real IDs.  Otherwise, use the
	 * effective IDs. */
	gid = proc_info(msg->from, access ? GID : EGID);
	uid = proc_info(msg->from, access ? UID : EUID);

	/* At first, grant only default (others') access. */
	permitted = (inode_ptr->i_mode & EXT2_S_IRWXO) >> 0;

	/* If the current task belongs to the group owning the inode, grant
	 * group access. */
	if (gid == inode_ptr->i_gid)
		permitted = (inode_ptr->i_mode & EXT2_S_IRWXG) >> 3;

	/* If the current task belongs to the user owning the inode, grant user
	 * access. */
	if (uid == inode_ptr->i_uid)
		permitted = (inode_ptr->i_mode & EXT2_S_IRWXU) >> 6;

	/* If the current task belongs to the super user, grant access! */
	if (super_user)
		return true;

	/* If the requested access is a subset of the permitted access, return
	 * true.  Otherwise, return false. */
	return (requested | permitted) == permitted;
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_access()				      |
\*----------------------------------------------------------------------------*/
int do_fs_access(const char *path, int amode)
{
	inode_t *inode_ptr = path_to_inode(path);
	bool permitted = amode == F_OK ? true : perm(inode_ptr, amode, true);

	if (inode_ptr == NULL)
	{
		/* The path name could not be resolved. */
		inode_put(inode_ptr);
		return -err_code;
	}

	inode_put(inode_ptr);
	return permitted ? 0 : -(err_code = EACCES);
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_chmod()				      |
\*----------------------------------------------------------------------------*/
int do_fs_chmod(const char *path, mode_t mode)
{
	inode_t *inode_ptr = path_to_inode(path);

	if (inode_ptr == NULL)
	{
		/* The path name could not be resolved. */
		inode_put(inode_ptr);
		return -err_code;
	}
	if (!super_user && proc_info(msg->from, EUID) != inode_ptr->i_uid)
	{
		/* No chmod permission. */
		inode_put(inode_ptr);
		return -(err_code = EPERM);
	}

	inode_ptr->i_mode = mode;
	if (!super_user && proc_info(msg->from, EGID) != inode_ptr->i_gid)
		inode_ptr->i_mode &= ~EXT2_S_ISGID;
	inode_ptr->i_ctime = do_time(NULL);
	inode_ptr->dirty = true;
	inode_put(inode_ptr);
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_chown()				      |
\*----------------------------------------------------------------------------*/
int do_fs_chown(const char *path, uid_t owner, gid_t group)
{
	inode_t *inode_ptr = path_to_inode(path);

	if (inode_ptr == NULL)
	{
		/* The path name could not be resolved. */
		inode_put(inode_ptr);
		return -err_code;
	}
	if (!super_user && proc_info(msg->from, EUID) != inode_ptr->i_uid)
	{
		/* No chown permission. */
		inode_put(inode_ptr);
		return -(err_code = EPERM);
	}
	if (!super_user && proc_info(msg->from, EUID) != owner)
	{
		/* No chown permission. */
		inode_put(inode_ptr);
		return -(err_code = EPERM);
	}

	inode_ptr->i_uid = owner == (uid_t) -1 ? inode_ptr->i_uid : owner;
	inode_ptr->i_gid = group == (gid_t) -1 ? inode_ptr->i_gid : group;
	if (owner != (uid_t) -1 || group != (gid_t) -1)
	{
		inode_ptr->i_ctime = do_time(NULL);
		inode_ptr->dirty = true;
	}
	inode_put(inode_ptr);
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_umask()				      |
\*----------------------------------------------------------------------------*/
mode_t do_fs_umask(mode_t cmask)
{
	mode_t old_cmask = fs_proc[msg->from].cmask;
	mode_t perm_bits = S_ISUID | S_ISGID | S_IRWXU | S_IRWXG | S_IRWXO;
	cmask &= perm_bits;

	fs_proc[msg->from].cmask = (old_cmask & ~perm_bits) | cmask;
	return old_cmask;
}
