/*----------------------------------------------------------------------------*\
 |	utime.h - access and modification times structure		      |
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

#ifndef _UTIME_H
#define _UTIME_H

/* Used for times in seconds: */
#ifndef _TIME_T
#define _TIME_T
typedef unsigned long time_t;
#endif

/* Data structure returned by utime(): */
struct utimbuf
{
	time_t actime;  /* Access time.       */
	time_t modtime; /* Modification time. */
};

/* Function prototype: */
int utime(const char *path, const struct utimbuf *times);

#endif
