/*----------------------------------------------------------------------------*\
 |	stat.h								      |
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

#ifndef _FS_STAT_H
#define _FS_STAT_H

#include <fs/inode.h>
#include <sys/stat.h>

/* Function prototypes: */
void fill_stat_buf(inode_t *inode_ptr, struct stat *buf);
int do_fs_fstat(int fildes, struct stat *buf);
int do_fs_stat(const char *path, struct stat *buf);

#endif
