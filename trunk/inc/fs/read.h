/*----------------------------------------------------------------------------*\
 |	read.h								      |
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

#ifndef _FS_READ_H
#define _FS_READ_H

#include <sys/types.h>

/* Function prototypes: */
ssize_t rw_fildes(bool read, int fildes, void *buf, size_t nbyte);
ssize_t do_fs_read(int fildes, void *buf, size_t nbyte);
ssize_t do_fs_write(int fildes, void *buf, size_t nbyte);

#endif
