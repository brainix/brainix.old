/*----------------------------------------------------------------------------*\
 |	perm.h								      |
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

#ifndef _FS_PERM_H
#define _FS_PERM_H

#include <stdbool.h>
#include <fs/inode.h>
#include <sys/types.h>

/* Function prototypes: */
bool perm(inode_t *inode_ptr, unsigned char requested, bool access);
int do_fs_access(const char *path, int amode);
int do_fs_chmod(const char *path, mode_t mode);
int do_fs_chown(const char *path, uid_t owner, gid_t group);
mode_t do_fs_umask(mode_t cmask);

#endif
