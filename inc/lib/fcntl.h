/*----------------------------------------------------------------------------*\
 |	fcntl.h - file control options					      |
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

#ifndef _FCNTL_H
#define _FCNTL_H

/* Values for cmd used by fcntl(): */
#define F_DUPFD		0 /* Duplicate file descriptor.                       */
#define F_GETFD		1 /* Get file descriptor flags.                       */
#define F_SETFD		2 /* Set file descriptor flags.                       */
#define F_GETFL		3 /* Get file status flags and file access modes.     */
#define F_SETFL		4 /* Set file status flags.                           */
#define F_GETLK		5 /* Get record locking information.                  */
#define F_SETLK		6 /* Set record locking information.                  */
#define F_SETLKW	7 /* Set record locking information; wait if blocked. */
#define F_GETOWN	8 /* Get process or process group to receive SIGURGs. */
#define F_SETOWN	9 /* Set process or process group to receive SIGURGs. */

/* File descriptor flags used for fcntl(): */
#define FD_CLOEXEC	0 /* Close file descriptor upon exec family function. */

/* Values for l_type used for record locking with fcntl(): */
#define F_UNLCK		0 /* Unlock.                  */
#define F_RDLCK		1 /* Shared or read lock.     */
#define F_WRLCK		2 /* Exclusive or write lock. */

/* Constants for lseek() and fcntl() functions: */
#define SEEK_CUR	0 /* Set file offset to current plus offset. */
#define SEEK_END	1 /* Set file offset to EOF plus offset.     */
#define SEEK_SET	2 /* Set file offset to offset.              */

/* File creation flags used in oflag value to open(): */
#define O_CREAT			0x0200 /* Create file if it does not exist.   */
#define O_EXCL			0x0100 /* Exclusive use flag.                 */
#define O_NOCTTY		0x0080 /* Do not assign controlling terminal. */
#define O_TRUNC			0x0040 /* Truncate flag.                      */
/* File status flags used for open() and fcntl(): */
#define O_APPEND		0x0020 /* Set append mode.                    */
#define O_NONBLOCK		0x0010 /* Non-blocking mode.                  */
#define O_SYNC			0x0008 /* Synchronize I/O file integrity .    */
/* File access modes for open() and fcntl(): */
#define O_ACCMODE		0x0007 /* Mask for file access modes.         */
	#define O_RDONLY	0x0004 /* Open for reading only.              */
	#define O_WRONLY	0x0002 /* Open for writing only.              */
	#define O_RDWR		0x0001 /* Open for reading and writing.       */

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

/* Used for file attributes: */
#ifndef _MODE_T
#define _MODE_T
typedef unsigned long mode_t;
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

/* File lock: */
struct flock
{
	short l_type;   /* Type of lock; F_UNLCK, F_RDLCK, F_WRLCK.     */
	short l_whence; /* Flag for starting offset.                    */
	off_t l_start;  /* Relative offset in bytes.                    */
	off_t l_len;    /* Size; if 0 then until EOF.                   */
	pid_t l_pid;    /* Process holding lock; returned with F_GETLK. */
};

#endif
