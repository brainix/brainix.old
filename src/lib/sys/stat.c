/*----------------------------------------------------------------------------*\
 |	stat.c								      |
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

#include <kernel/syscall.h>
#include <sys/stat.h>

/*----------------------------------------------------------------------------*\
 |				    chmod()				      |
\*----------------------------------------------------------------------------*/
int chmod(const char *path, mode_t mode)
{

/* Change mode of a file. */

	return syscall(SYS_CHMOD, (unsigned long) path, mode, 0);
}

/*----------------------------------------------------------------------------*\
 |				    fstat()				      |
\*----------------------------------------------------------------------------*/
int fstat(int fildes, struct stat *buf)
{

/* Get file status. */

	return syscall(SYS_FSTAT, fildes, (unsigned long) buf, 0);
}

/*----------------------------------------------------------------------------*\
 |				     stat()				      |
\*----------------------------------------------------------------------------*/
int stat(const char *path, struct stat *buf)
{

/* Get file status. */

	return syscall(SYS_STAT, (unsigned long) path, (unsigned long) buf, 0);
}

/*----------------------------------------------------------------------------*\
 |				    umask()				      |
\*----------------------------------------------------------------------------*/
mode_t umask(mode_t cmask)
{

/* Set and get the file mode creation mask. */

	return syscall(SYS_UMASK, cmask, 0, 0);
}
