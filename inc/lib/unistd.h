/*----------------------------------------------------------------------------*\
 |	unistd.h - standard symbolic constants and types		      |
 |									      |
 |	Copyright © 2002-2006, Team Brainix, original authors.		      |
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



#ifndef _UNISTD_H
#define _UNISTD_H



/*----------------------------------------------------------------------------*\
 |			      Version Test Macros			      |
\*----------------------------------------------------------------------------*/

#define _POSIX_VERSION	200112L
#define _POSIX2_VERSION	200112L
#define _XOPEN_VERSION	600



/*----------------------------------------------------------------------------*\
 |		    Constants for Options and Option Groups		      |
\*----------------------------------------------------------------------------*/

#define _POSIX_NO_TRUNC	1



/*----------------------------------------------------------------------------*\
 |			    Constants for Functions			      |
\*----------------------------------------------------------------------------*/

/* Null pointer: */
#define NULL		((void *) 0)

/* Constants for access() function: */
#define F_OK		0x0000 /* Test for existence of file.         */
#define R_OK		0x0004 /* Test for read permission.           */
#define W_OK		0x0002 /* Test for write permission.          */
#define X_OK		0x0001 /* Test for execute/search permission. */

/* Constants for lseek() and fcntl() functions: */
#define SEEK_CUR	0 /* Set file offset to current plus offset. */
#define SEEK_END	1 /* Set file offset to EOF plus offset.     */
#define SEEK_SET	2 /* Set file offset to offset.              */

/* Constants for file streams: */
#define STDIN_FILENO	0 /* File number of stdin.  */
#define STDOUT_FILENO	1 /* File number of stdout. */
#define STDERR_FILENO	2 /* File number of stderr. */



/*----------------------------------------------------------------------------*\
 |				Type Definitions			      |
\*----------------------------------------------------------------------------*/

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

/* Used for user IDs: */
#ifndef _UID_T
#define _UID_T
typedef unsigned long uid_t;
#endif

/* Used for group IDs: */
#ifndef _GID_T
#define _GID_T
typedef unsigned long gid_t;
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



/*----------------------------------------------------------------------------*\
 |				  Declarations				      |
\*----------------------------------------------------------------------------*/

/* Function prototypes: */
int access(const char *path, int amode);
unsigned alarm(unsigned seconds);
int chdir(const char *path);
int chown(const char *path, uid_t owner, gid_t group);
int close(int fildes);
int dup(int fildes);
int dup2(int fildes, int fildes2);
int execve(const char *path, char *const argv[], char *const envp[]);
pid_t fork(void);
gid_t getgid(void);
pid_t getpgrp(void);
pid_t getpid(void);
uid_t getuid(void);
int link(const char *path1, const char *path2);
off_t lseek(int fildes, off_t offset, int whence);
int pause(void);
ssize_t read(int fildes, void *buf, size_t nbyte);
int setgid(gid_t gid);
pid_t setsid(void);
int setuid(uid_t uid);
void sync(void);
int unlink(const char *path);
ssize_t write(int fildes, const void *buf, size_t nbyte);



#endif
