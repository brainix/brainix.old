/*----------------------------------------------------------------------------*\
 |	times.h - data returned by the times() function			      |
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

#ifndef _SYS_TIMES_H
#define _SYS_TIMES_H

/* Used for system times in clock ticks: */
#ifndef _CLOCK_T
#define _CLOCK_T
typedef unsigned long clock_t;
#endif

/* Data structure returned by times(): */
struct tms
{
	clock_t tms_utime;  /* User CPU time.                                 */
	clock_t tms_stime;  /* System CPU time.                               */
	clock_t tms_cutime; /* User CPU time of terminated child processes.   */
	clock_t tms_cstime; /* System CPU time of terminated child processes. */
};

/* Function prototype: */
clock_t times(struct tms *buffer);

#endif
