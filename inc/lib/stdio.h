/*----------------------------------------------------------------------------*\
 |	stdio.h - standard buffered input/output			      |
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

#ifndef _STDIO_H
#define _STDIO_H

#define SEEK_CUR	0 /* Seek relative to current position. */
#define SEEK_END	1 /* Seek relative to end-of-file.      */
#define SEEK_SET	2 /* Seek relative to start-of-file.    */

/* Null pointer: */
#define NULL		((void *) 0)

typedef struct
{
	int fildes;
} FILE;

/* Used for sizes of objects: */
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif

#endif
