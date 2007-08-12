/*----------------------------------------------------------------------------*\
 |	syscall.h							      |
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

#ifndef _KERNEL_SYSCALL_H
#define _KERNEL_SYSCALL_H

/*----------------------------------------------------------------------------*\
 |				Syscall Numbers				      |
\*----------------------------------------------------------------------------*/

/* Process management: */
#define SYS_SBRK 	 1 /* Change data segment size.                 */
#define SYS_FORK 	 2 /* Create new process.                       */
#define SYS_EXECVE 	 3 /* Execute file.                             */
#define SYS_EXIT 	 4 /* Terminate process.                        */
#define SYS_GETPGRP 	 5 /* Get process group ID.                     */
#define SYS_GETPID 	 6 /* Get process ID.                           */
#define SYS_SETSID 	 7 /* Create session & set process group ID.    */
#define SYS_WAIT 	 8 /* Wait for child process to stop/terminate. */
#define SYS_WAITPID 	 9 /* Wait for child process to stop/terminate. */

/* Signals: */
#define SYS_ALARM	10 /* Schedule alarm signal.                   */
#define SYS_KILL	11 /* Send signal to process or process group. */
#define SYS_PAUSE	12 /* Suspend thread until signal is received. */
#define SYS_SIGACTION	13 /* Examine & change signal action.          */
#define SYS_SIGPENDING	14 /* Examine pending signals.                 */
#define SYS_SIGPROCMASK	15 /* Examine & change blocked signals.        */

/* File management: */
#define SYS_ACCESS	16 /* Determine accessibility of file.              */
#define SYS_CLOSE	17 /* Close file descriptor.                        */
#define SYS_CREAT	18 /* Create new file / rewrite existing file.      */
#define SYS_DUP		19 /* Duplicate open file descriptor.               */
#define SYS_DUP2	20 /* Duplicate open file descriptor.               */
#define SYS_FCNTL	21 /* File control.                                 */
#define SYS_FSTAT	22 /* Get file status.                              */
#define SYS_IOCTL	23 /* Control STREAMS device.                       */
#define SYS_LSEEK	24 /* Move read/write file offset.                  */
#define SYS_MKNOD	25 /* Make directory / special file / regular file. */
#define SYS_OPEN	26 /* Open file.                                    */
#define SYS_PIPE	27 /* Create interprocess channel.                  */
#define SYS_READ	28 /* Read from file.                               */
#define SYS_RENAME	29 /* Rename file.                                  */
#define SYS_STAT	30 /* Get file status.                              */
#define SYS_WRITE	31 /* Write on file.                                */

/* Directory & file system management: */
#define SYS_CHDIR	32 /* Change working directory.     */
#define SYS_CHROOT	33 /* Change root directory.        */
#define SYS_LINK	34 /* Link to file.                 */
#define SYS_MKDIR	35 /* Make directory.               */
#define SYS_RMDIR	36 /* Remove directory.             */
#define SYS_SYNC	37 /* Schedule file system updates. */
#define SYS_UNLINK	38 /* Remove directory entry.       */

/* Protection: */
#define SYS_CHMOD	39 /* Change mode of file.               */
#define SYS_CHOWN	40 /* Change owner & group of file.      */
#define SYS_GETGID	41 /* Get real group ID.                 */
#define SYS_GETUID	42 /* Get real user ID.                  */
#define SYS_SETGID	43 /* Set group ID.                      */
#define SYS_SETUID	44 /* Set user ID.                       */
#define SYS_UMASK	45 /* Set & get file mode creation mask. */

/* Time management: */
#define SYS_TIME	46 /* Get time.                                     */
#define SYS_TIMES	47 /* Get process & waited-for child process times. */
#define SYS_UTIME	48 /* Set file access & modification times.         */

/* Brainix-specific: */
#define REGISTER	49 /* */
#define IRQ		50 /* */
#define FREQUENCY	51 /* */
#define WATCHDOG	52 /* */
#define UPTIME		53 /* */
#define SHUTDOWN	54 /* */

unsigned long syscall(unsigned char num,
		      unsigned long arg1,
		      unsigned long arg2,
		      unsigned long arg3);

#endif
