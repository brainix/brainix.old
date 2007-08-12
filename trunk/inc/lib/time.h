/*----------------------------------------------------------------------------*\
 |	time.h - time types						      |
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

#ifndef _TIME_H
#define _TIME_H

struct tm
{
	int tm_sec;   /* Seconds               [0, 60] (          ). */
	int tm_min;   /* Minutes               [0, 59] (          ). */
	int tm_hour;  /* Hour                  [0, 23] (          ). */
	int tm_mday;  /* Day of month          [1, 31] (          ). */
	int tm_mon;   /* Month of year         [0, 11] (          ). */
	int tm_year;  /* Years since 1900      [     ] (          ). */
	int tm_wday;  /* Day of week           [0,  6] (Sunday = 0). */
	int tm_yday;  /* Day of year           [0,365] (          ). */
	int tm_isdst; /* Daylight Savings flag [     ] (          ). */
};

#define NULL	((void *) 0)

/* Used for system times in clock ticks: */
#ifndef _CLOCK_T
#define _CLOCK_T
typedef unsigned long clock_t;
#endif

/* Used for sizes of objects: */
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif

/* Used for times in seconds: */
#ifndef _TIME_T
#define _TIME_T
typedef unsigned long time_t;
#endif

/* Function prototype: */
time_t time(time_t *tloc);

#endif
