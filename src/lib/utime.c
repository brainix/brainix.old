/*----------------------------------------------------------------------------*\
 |	utime.c								      |
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

#include <kernel/syscall.h>
#include <utime.h>

/*----------------------------------------------------------------------------*\
 |				    utime()				      |
\*----------------------------------------------------------------------------*/
int utime(const char *path, const struct utimbuf *times)
{

/* Set file access and modification times. */

	return syscall(SYS_UTIME,
		       (unsigned long) path,
		       (unsigned long) times,
		       0);
}
