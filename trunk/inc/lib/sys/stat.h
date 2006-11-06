/*----------------------------------------------------------------------------*\
 |	stat.h - data returned by the stat() function			      |
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

#ifndef _SYS_STAT_H
#define _SYS_STAT_H

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

/* Used for device IDs: */
#ifndef _DEV_T
#define _DEV_T
typedef unsigned long dev_t;
#endif

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

/* Used for times in seconds: */
#ifndef _TIME_T
#define _TIME_T
typedef unsigned long time_t;
#endif

/* Data structure returned by fstat(), lstat(), and stat(): */
struct stat
{
	dev_t st_dev;     /* Device ID of device containing file. */
	ino_t st_ino;     /* File serial number.                  */
	mode_t st_mode;   /* Mode of file.                        */
	nlink_t st_nlink; /* Number of hard links to file.        */
	uid_t st_uid;     /* User ID of file.                     */
	gid_t st_gid;     /* Group ID of file.                    */
	off_t st_size;    /* File size in bytes.                  */
	time_t st_atime;  /* Time of last access.                 */
	time_t st_mtime;  /* Time of last data modification.      */
	time_t st_ctime;  /* Time of last status change.          */
};

/* Values of type mode_t (file type): */
#define S_IFMT			0xF000 /* Type of file.      */
	#define S_IFSOCK	0xC000 /* Socket.            */
	#define S_IFLNK		0xA000 /* Symbolic link.     */
	#define S_IFREG		0x8000 /* Regular.           */
	#define S_IFBLK		0x6000 /* Block special.     */
	#define S_IFDIR		0x4000 /* Directory.         */
	#define S_IFCHR		0x2000 /* Character special. */
	#define S_IFIFO		0x1000 /* FIFO special.      */

/* Values of type mode_t (file mode bits): */
#define S_ISUID		0x0800 /* Set-user-ID on execution.                 */
#define S_ISGID		0x0400 /* Set-group-ID on execution.                */
#define S_ISVTX		0x0200 /* On directories, restricted deletion flag. */
#define S_IRWXU		0x01C0 /* Read, write, execute/search by owner.     */
	#define S_IRUSR	0x0100 /* Read permission, owner.                   */
	#define S_IWUSR	0x0080 /* Write permission, owner.                  */
	#define S_IXUSR	0x0040 /* Execute/search permission, owner.         */
#define S_IRWXG		0x0038 /* Read, write, execute/search by group.     */
	#define S_IRGRP	0x0020 /* Read permission, group.                   */
	#define S_IWGRP	0x0010 /* Write permission, group.                  */
	#define S_IXGRP	0x0008 /* Execute/search permission, group.         */
#define S_IRWXO		0x0007 /* Read, write, execute/search by others.    */
	#define S_IROTH	0x0004 /* Read permission, others.                  */
	#define S_IWOTH	0x0002 /* Write permission, others.                 */
	#define S_IXOTH	0x0001 /* Execute/search permission, others.        */

/* Function prototypes: */
int chmod(const char *path, mode_t mode);
int fstat(int fildes, struct stat *buf);
int stat(const char *path, struct stat *buf);
mode_t umask(mode_t cmask);

#endif
