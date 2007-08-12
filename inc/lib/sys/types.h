/*----------------------------------------------------------------------------*\
 |	types.h - data types						      |
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

#ifndef _SYS_TYPES_H
#define _SYS_TYPES_H

/* Used for file block counts: */
#ifndef _BLKCNT_T
#define _BLKCNT_T
typedef long blkcnt_t;
#endif

/* Used for block sizes: */
#ifndef _BLKSIZE_T
#define _BLKSIZE_T
typedef long blksize_t;
#endif

/* Used for system times in clock ticks: */
#ifndef _CLOCK_T
#define _CLOCK_T
typedef unsigned long clock_t;
#endif

/* Used for device IDs: */
#ifndef _DEV_T
#define _DEV_T
typedef unsigned long dev_t;
#endif

/* Used for group IDs: */
#ifndef _GID_T
#define _GID_T
typedef unsigned long gid_t;
#endif

/* Used for general identifiers (PIDs, UIDs, or GIDs): */
typedef unsigned long id_t;

/* Used for file serial numbers: */
#ifndef _INO_T
#define _INO_T
typedef unsigned long ino_t;
#endif

/* Used for file attributes: */
#ifndef _MODE_T
#define _MODE_T
typedef unsigned long mode_t;
#endif

/* Used for link counts: */
#ifndef _NLINK_T
#define _NLINK_T
typedef unsigned short nlink_t;
#endif

/* Used for file sizes: */
#ifndef _OFF_T
#define _OFF_T
typedef long off_t;
#endif

/* Used for process and process group IDs: */
#ifndef _PID_T
#define _PID_T
typedef long pid_t;
#endif

/* Used for sizes of objects: */
#ifndef _SIZE_T
#define _SIZE_T
typedef unsigned long size_t;
#endif

/* Used for byte counts or error indications: */
#ifndef _SSIZE_T
#define _SSIZE_T
typedef long ssize_t;
#endif

/* Used for times in seconds: */
#ifndef _TIME_T
#define _TIME_T
typedef unsigned long time_t;
#endif

/* Used for user IDs: */
#ifndef _UID_T
#define _UID_T
typedef unsigned long uid_t;
#endif

#endif
