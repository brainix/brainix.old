/*----------------------------------------------------------------------------*\
 |	unistd.c							      |
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

#include <kernel/syscall.h>
#include <unistd.h>

/*----------------------------------------------------------------------------*\
 |				    access()				      |
\*----------------------------------------------------------------------------*/
int access(const char *path, int amode)
{

/* Determine accessibility of a file. */

	return syscall(SYS_ACCESS, (unsigned long) path, amode, 0);
}

/*----------------------------------------------------------------------------*\
 |				    alarm()				      |
\*----------------------------------------------------------------------------*/
unsigned alarm(unsigned seconds)
{

/* Schedule an alarm signal. */

	return syscall(SYS_ALARM, seconds, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				    chdir()				      |
\*----------------------------------------------------------------------------*/
int chdir(const char *path)
{

/* Change working directory. */

	return syscall(SYS_CHDIR, (unsigned long) path, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				    chown()				      |
\*----------------------------------------------------------------------------*/
int chown(const char *path, uid_t owner, gid_t group)
{

/* Change owner and group of a file. */

	return syscall(SYS_CHOWN, (unsigned long) path, owner, group);
}

/*----------------------------------------------------------------------------*\
 |				    close()				      |
\*----------------------------------------------------------------------------*/
int close(int fildes)
{

/* Close a file descriptor. */

	return syscall(SYS_CLOSE, fildes, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				     dup()				      |
\*----------------------------------------------------------------------------*/
int dup(int fildes)
{

/* Duplicate an open file descriptor. */

	return syscall(SYS_DUP, fildes, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				     dup2()				      |
\*----------------------------------------------------------------------------*/
int dup2(int fildes, int fildes2)
{

/* Duplicate an open file descriptor. */

	return syscall(SYS_DUP2, fildes, fildes2, 0);
}

/*----------------------------------------------------------------------------*\
 |				    execve()				      |
\*----------------------------------------------------------------------------*/
int execve(const char *path, char *const argv[], char *const envp[])
{

/* Execute a file. */

	return syscall(SYS_EXECVE,
		       (unsigned long) path,
		       (unsigned long) argv,
		       (unsigned long) envp);
}

/*----------------------------------------------------------------------------*\
 |				     fork()				      |
\*----------------------------------------------------------------------------*/
pid_t fork(void)
{

/* Create a new process. */

	return syscall(SYS_FORK, 0, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				    getgid()				      |
\*----------------------------------------------------------------------------*/
gid_t getgid(void)
{

/* Get the real group ID. */

	return syscall(SYS_GETGID, 0, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				   getpgrp()				      |
\*----------------------------------------------------------------------------*/
pid_t getpgrp(void)
{

/* Get the process group ID of the calling process. */

	return syscall(SYS_GETPGRP, 0, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				    getpid()				      |
\*----------------------------------------------------------------------------*/
pid_t getpid(void)
{

/* Get the process ID. */

	return syscall(SYS_GETPID, 0, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				    getuid()				      |
\*----------------------------------------------------------------------------*/
uid_t getuid(void)
{

/* Get a real user ID. */

	return syscall(SYS_GETUID, 0, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				     link()				      |
\*----------------------------------------------------------------------------*/
int link(const char *path1, const char *path2)
{

/* Link to a file. */

	return syscall(SYS_LINK,
		       (unsigned long) path1,
		       (unsigned long) path2,
		       0);
}

/*----------------------------------------------------------------------------*\
 |				    lseek()				      |
\*----------------------------------------------------------------------------*/
off_t lseek(int fildes, off_t offset, int whence)
{

/* Move the read/write file offset. */

	return syscall(SYS_LSEEK, fildes, offset, whence);
}

/*----------------------------------------------------------------------------*\
 |				    pause()				      |
\*----------------------------------------------------------------------------*/
int pause(void)
{

/* Suspend the thread until a signal is received. */

	return syscall(SYS_PAUSE, 0, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				     read()				      |
\*----------------------------------------------------------------------------*/
ssize_t read(int fildes, void *buf, size_t nbyte)
{

/* Read from a file. */

	return syscall(SYS_READ, fildes, (unsigned long) buf, nbyte);
}

/*----------------------------------------------------------------------------*\
 |				    setgid()				      |
\*----------------------------------------------------------------------------*/
int setgid(gid_t gid)
{

/* Set-group-ID. */

	return syscall(SYS_SETGID, gid, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				    setsid()				      |
\*----------------------------------------------------------------------------*/
pid_t setsid(void)
{

/* Create session and set process group ID. */

	return syscall(SYS_SETSID, 0, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				    setuid()				      |
\*----------------------------------------------------------------------------*/
int setuid(uid_t uid)
{

/* Set user ID. */

	return syscall(SYS_SETUID, uid, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				     sync()				      |
\*----------------------------------------------------------------------------*/
void sync(void)
{

/* Schedule file system updates. */

	syscall(SYS_SYNC, 0, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				    unlink()				      |
\*----------------------------------------------------------------------------*/
int unlink(const char *path)
{

/* Remove a directory entry. */

	return syscall(SYS_UNLINK, (unsigned long) path, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				    write()				      |
\*----------------------------------------------------------------------------*/
ssize_t write(int fildes, const void *buf, size_t nbyte)
{

/* Write on a file. */

	return syscall(SYS_WRITE, fildes, (unsigned long) buf, nbyte);
}
