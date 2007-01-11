/*----------------------------------------------------------------------------*\
 |	stddef.h - standard type definitions				      |
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

#ifndef _STDDEF_H
#define _STDDEF_H

#define NULL	((void *) 0)

#define offsetof(type, member_designator) \
	((size_t) &((type *) 0)->member_designator)

typedef long ptrdiff_t;

/* Used for sizes of objects: */
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif

#endif
