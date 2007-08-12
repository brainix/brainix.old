/*----------------------------------------------------------------------------*\
 |	dirent.h - format of directory entries				      |
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

#ifndef _DIRENT_H
#define _DIRENT_H

/* Used for file serial numbers: */
#ifndef _INO_T
#define _INO_T
typedef unsigned long ino_t;
#endif

/* Directory entry: */
struct dirent
{
	ino_t d_ino;          /* File serial number. */
	char d_name[255 + 1]; /* Name of entry.      */
};

#endif
