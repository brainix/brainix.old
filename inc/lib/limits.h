/*----------------------------------------------------------------------------*\
 |	limits.h - implementation-defined constants			      |
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



#ifndef _LIMITS_H
#define _LIMITS_H



/*----------------------------------------------------------------------------*\
 |	       Runtime Invariant Values (Possibly Indeterminate)	      |
\*----------------------------------------------------------------------------*/

/* Max number of files a process can have open at a time: */
#define OPEN_MAX		_POSIX_OPEN_MAX

/* Max number of symlinks that can be traversed during pathname resolution: */
#define SYMLOOP_MAX		_POSIX_SYMLOOP_MAX



/*----------------------------------------------------------------------------*\
 |			   Path Name Variable Values			      |
\*----------------------------------------------------------------------------*/

/* Max number of links to a single file: */
#define LINK_MAX		_POSIX_LINK_MAX

/* Max number of bytes in a file name (not including terminating null): */
#define NAME_MAX		_XOPEN_NAME_MAX

/* Max number of bytes in a path name (including terminating null): */
#define PATH_MAX		_XOPEN_PATH_MAX



/*----------------------------------------------------------------------------*\
 |				 Minimum Values				      |
\*----------------------------------------------------------------------------*/

/* Max number of links to a single file: */
#define _POSIX_LINK_MAX		8

/* Max number of files a process can have open at a time: */
#define _POSIX_OPEN_MAX		20

/* The value that can be stored in an object of type ssize_t: */
#define _POSIX_SSIZE_MAX	32767

/* Max number of symlinks that can be traversed during pathname resolution: */
#define _POSIX_SYMLOOP_MAX	8

/* Max number of bytes in a file name (not including terminating null): */
#define _XOPEN_NAME_MAX		255

/* Max number of bytes in a path name (including terminating null): */
#define _XOPEN_PATH_MAX		1024



/*----------------------------------------------------------------------------*\
 |				Numerical Limits			      |
\*----------------------------------------------------------------------------*/

#define CHAR_BIT	8
#define CHAR_MIN	((char)                -128)
#define CHAR_MAX	((char)                 127)
#define SCHAR_MIN	((signed char)         -128)
#define SCHAR_MAX	((signed char)          127)
#define UCHAR_MAX	((unsigned char)        255)
#define SHRT_MIN	((short)             -32767)
#define SHRT_MAX	((short)              32767)
#define USHRT_MAX	((unsigned short)     65535)
#define INT_MIN		((int)          -2147483647)
#define INT_MAX		((int)           2147483647)
#define UINT_MAX	((unsigned int)  4294967295)
#define LONG_MIN	((long)         -2147483647)
#define LONG_MAX	((long)          2147483647)
#define ULONG_MAX	((unsigned long) 4294967295)
#define SSIZE_MAX	((ssize_t) _POSIX_SSIZE_MAX)
#define MB_LEN_MAX	1



#endif
