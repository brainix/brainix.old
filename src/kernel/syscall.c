/*----------------------------------------------------------------------------*\
 |	syscall.c							      |
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

#define BRAINIX
#include <kernel/kernel.h>

/* Function prototypes: */
void do_syscall(stack_t s);
int cleanup(msg_t *msg);
void leaky_copy(char ***p1, char *const **p2);
void leaky_free(char ***p);
pid_t do_fork(void);
int do_execve(const char *path, char *const argv[], char *const envp[]);
void do_exit(int status);
gid_t do_getegid(void);
uid_t do_geteuid(void);
gid_t do_getgid(void);
pid_t do_getpgrp(void);
pid_t do_getpid(void);
pid_t do_getppid(void);
uid_t do_getuid(void);
pid_t do_setpgrp(void);
unsigned do_alarm(unsigned seconds);
int do_access(const char *path, int amode);
int do_close(int fildes);
int do_dup(int fildes);
int do_dup2(int fildes, int fildes2);
int do_fcntl(int fildes, int cmd, int arg);
int do_fstat(int fildes, struct stat *buf);
int do_ioctl(int fildes, int request, int arg);
off_t do_lseek(int fildes, off_t offset, int whence);
int do_open(const char *path, int oflag, mode_t mode);
ssize_t do_read(int fildes, void *buf, size_t nbyte);
int do_stat(const char *path, struct stat *buf);
ssize_t do_write(int fildes, const void *buf, size_t nbyte);
int do_chdir(const char *path);
int do_chroot(const char *path);
int do_link(const char *path1, const char *path2);
void do_sync(void);
int do_unlink(const char *path);
int do_chmod(const char *path, mode_t mode);
int do_chown(const char *path, uid_t owner, gid_t group);
mode_t do_umask(mode_t cmask);
time_t do_time(time_t *tloc);
int do_utime(const char *path, const struct utimbuf *times);

/*----------------------------------------------------------------------------*\
 |				  do_syscall()				      |
\*----------------------------------------------------------------------------*/
void do_syscall(stack_t s)
{
	proc_time(KERNEL);

	switch (s.eax)
	{
		case SYS_SBRK:
			s.eax = (unsigned long) do_sbrk(s.ecx);
			break;
		case SYS_FORK:
			s.eax = do_fork();
			break;
		case SYS_EXECVE:
			s.eax = do_execve((char *) s.ecx,
					  (char **) s.edx,
					  (char **) s.ebx);
			break;
		case SYS_EXIT:
			do_exit(s.ecx);
			break;
		case SYS_GETPGRP:
			s.eax = do_getpgrp();
			break;
		case SYS_GETPID:
			s.eax = do_getpid();
			break;
		case SYS_SETSID:
			break;
		case SYS_WAIT:
			break;
		case SYS_WAITPID:
			break;

		case SYS_ALARM:
			s.eax = do_alarm(s.ecx);
			break;
		case SYS_KILL:
			s.eax = do_kill(s.ecx, s.edx);
			break;
		case SYS_PAUSE:
			s.eax = do_pause();
			break;
		case SYS_SIGACTION:
			s.eax = do_sigaction(s.ecx,
					     (struct sigaction *) s.edx,
					     (struct sigaction *) s.ebx);
			break;
		case SYS_SIGPENDING:
			s.eax = do_sigpending((sigset_t *) s.ecx);
			break;
		case SYS_SIGPROCMASK:
			s.eax = do_sigprocmask(s.ecx,
					       (sigset_t *) s.edx,
					       (sigset_t *) s.ebx);
			break;

		case SYS_ACCESS:
			s.eax = do_access((char *) s.ecx, s.edx);
			break;
		case SYS_CLOSE:
			s.eax = do_close(s.ecx);
			break;
		case SYS_CREAT:
			break;
		case SYS_DUP:
			s.eax = do_dup(s.ecx);
			break;
		case SYS_DUP2:
			s.eax = do_dup2(s.ecx, s.edx);
			break;
		case SYS_FCNTL:
			s.eax = do_fcntl(s.ecx, s.edx, s.ebx);
			break;
		case SYS_FSTAT:
			s.eax = do_fstat(s.ecx, (struct stat *) s.edx);
			break;
		case SYS_IOCTL:
			s.eax = do_ioctl(s.ecx, s.edx, s.ebx);
			break;
		case SYS_LSEEK:
			s.eax = do_lseek(s.ecx, s.edx, s.ebx);
			break;
		case SYS_MKNOD:
			break;
		case SYS_OPEN:
			s.eax = do_open((char *) s.ecx, s.edx, s.ebx);
			break;
		case SYS_PIPE:
			break;
		case SYS_READ:
			s.eax = do_read(s.ecx, (void *) s.edx, s.ebx);
			break;
		case SYS_RENAME:
			break;
		case SYS_STAT:
			s.eax = do_stat((char *) s.ecx, (struct stat *) s.edx);
			break;
		case SYS_WRITE:
			s.eax = do_write(s.ecx, (void *) s.edx, s.ebx);
			break;

		case SYS_CHDIR:
			s.eax = do_chdir((char *) s.ecx);
			break;
		case SYS_CHROOT:
			s.eax = do_chroot((char *) s.ecx);
			break;
		case SYS_LINK:
			s.eax = do_link((char *) s.ecx, (char *) s.edx);
			break;
		case SYS_MKDIR:
			break;
		case SYS_RMDIR:
			break;
		case SYS_SYNC:
			do_sync();
			break;
		case SYS_UNLINK:
			s.eax = do_unlink((char *) s.ecx);
			break;

		case SYS_CHMOD:
			s.eax = do_chmod((char *) s.ecx, s.edx);
			break;
		case SYS_CHOWN:
			s.eax = do_chown((char *) s.ecx, s.edx, s.ebx);
			break;
		case SYS_GETGID:
			s.eax = do_getgid();
			break;
		case SYS_GETUID:
			s.eax = do_getuid();
			break;
		case SYS_SETGID:
			break;
		case SYS_SETUID:
			break;
		case SYS_UMASK:
			s.eax = do_umask(s.ecx);
			break;

		case SYS_TIME:
			s.eax = do_time((time_t *) s.ecx);
			break;
		case SYS_TIMES:
			s.eax = do_times((struct tms *) s.ecx);
			break;
		case SYS_UTIME:
			s.eax = do_utime((char *) s.ecx,
					 (struct utimbuf *) s.edx);
			break;

		default:
			panic("do_syscall", "invalid opcode");
			break;
	}

	proc_time(USER);
}

/*----------------------------------------------------------------------------*\
 |				   cleanup()				      |
\*----------------------------------------------------------------------------*/
int cleanup(msg_t *msg)
{
	int ret_val = 0;

	switch (msg->op)
	{
		case SYS_FORK:   ret_val = msg->args.fork.ret_val;   break;
		case SYS_EXECVE: ret_val = msg->args.execve.ret_val; break;
		case SYS_ALARM:  ret_val = msg->args.alarm.ret_val;  break;
		case SYS_ACCESS: ret_val = msg->args.access.ret_val; break;
		case SYS_CLOSE:  ret_val = msg->args.close.ret_val;  break;
		case SYS_DUP:    ret_val = msg->args.dup.ret_val;    break;
		case SYS_DUP2:   ret_val = msg->args.dup2.ret_val;   break;
		case SYS_FCNTL:  ret_val = msg->args.fcntl.ret_val;  break;
		case SYS_FSTAT:  ret_val = msg->args.fstat.ret_val;  break;
		case SYS_IOCTL:  ret_val = msg->args.ioctl.ret_val;  break;
		case SYS_LSEEK:  ret_val = msg->args.lseek.ret_val;  break;
		case SYS_OPEN:   ret_val = msg->args.open.ret_val;   break;
		case SYS_READ:   ret_val = msg->args.read.ret_val;   break;
		case SYS_STAT:   ret_val = msg->args.stat.ret_val;   break;
		case SYS_WRITE:  ret_val = msg->args.write.ret_val;  break;
		case SYS_CHDIR:  ret_val = msg->args.chdir.ret_val;  break;
		case SYS_CHROOT: ret_val = msg->args.chroot.ret_val; break;
		case SYS_LINK:   ret_val = msg->args.link.ret_val;   break;
		case SYS_UNLINK: ret_val = msg->args.unlink.ret_val; break;
		case SYS_CHMOD:  ret_val = msg->args.chmod.ret_val;  break;
		case SYS_CHOWN:  ret_val = msg->args.chown.ret_val;  break;
		case SYS_UMASK:  ret_val = msg->args.umask.ret_val;  break;
		case SYS_TIME:   ret_val = msg->args.time.ret_val;   break;
		case SYS_UTIME:  ret_val = msg->args.utime.ret_val;  break;
	}

	msg_free(msg);
	if (ret_val < 0)
	{
		errno = -ret_val;
		ret_val = -1;
	}
	return ret_val;
}

/*----------------------------------------------------------------------------*\
 |				  leaky_copy()				      |
\*----------------------------------------------------------------------------*/
void leaky_copy(char ***p1, char *const **p2)
{
	int n;

	for (n = 0; *p2[n] != '\0'; n++)
		;
	*p1 = kmalloc((n + 1) * sizeof(char *));
	for (; n >= 0; n--)
	{
		*p1[n] = kmalloc(strlen(*p2[n]) + 1);
		strcpy(*p1[n], *p2[n]);
	}
}

/*----------------------------------------------------------------------------*\
 |				  leaky_free()				      |
\*----------------------------------------------------------------------------*/
void leaky_free(char ***p)
{
	int n;

	for (n = 0; *p[n] != '\0'; n++)
		kfree(*p[n]);
	kfree(*p[n]);
	kfree(*p);
}

/*----------------------------------------------------------------------------*\
 |				   do_fork()				      |
\*----------------------------------------------------------------------------*/
pid_t do_fork(void)
{

/* Perform the fork() system call.  God damn, Unix is goofy. */

	msg_t *msg = msg_alloc(KERNEL_PID, SYS_FORK);
	pid_t ret_val;

	/* Politely ask the µ-kernel to fork() and anxiously await the
	 * µ-kernel's reply. */
	msg_send_receive(msg);
	ret_val = msg->args.fork.ret_val;

	/* What the hell just happened? */
	if (ret_val < 0)
		/* The fork() failed.  There's nothing else to do. */
		return cleanup(msg);

	/* The fork() succeeded.  Who the hell are we? */
	if (ret_val == 0)
		/* We're the child.  There's nothing else to do. */
		return cleanup(msg);

	/* We're the parent.  Kindly inform the file system of the fork(). */
	msg->from = do_getpid();
	msg->to = FS_PID;
	msg_send(msg);
	return ret_val;
}

/*----------------------------------------------------------------------------*\
 |				  do_execve()				      |
\*----------------------------------------------------------------------------*/
int do_execve(const char *path, char *const argv[], char *const envp[])
{

/* Politely ask the file system to perform the execve() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_EXECVE);
	msg->args.execve.path = kmalloc(PATH_MAX);
	strcpy(msg->args.execve.path, path);
	leaky_copy(&msg->args.execve.argv, &argv);
	leaky_copy(&msg->args.execve.envp, &envp);

	msg_send_receive(msg);

	kfree(msg->args.execve.path);
	leaky_free(&msg->args.execve.argv);
	leaky_free(&msg->args.execve.envp);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_exit()				      |
\*----------------------------------------------------------------------------*/
void do_exit(int status)
{

/* Perform the exit() system call. */

	/* Inform the file system of the exit(). */
	msg_t *msg = msg_alloc(FS_PID, SYS_EXIT);
	msg->args.exit.status = status;
	msg_send(msg);

	/* Politely ask the µ-kernel to exit(). */
	msg = msg_alloc(KERNEL_PID, SYS_EXIT);
	msg->args.exit.status = status;
	msg_send(msg);

	/* Now it's up to the µ-kernel.  There's nothing else to do. */
	while (true)
		proc_nap();
}

/*----------------------------------------------------------------------------*\
 |				  do_getegid()				      |
\*----------------------------------------------------------------------------*/
gid_t do_getegid(void)
{
	return proc_info(CURRENT_TASK, EGID);
}

/*----------------------------------------------------------------------------*\
 |				  do_geteuid()				      |
\*----------------------------------------------------------------------------*/
uid_t do_geteuid(void)
{
	return proc_info(CURRENT_TASK, EUID);
}

/*----------------------------------------------------------------------------*\
 |				  do_getgid()				      |
\*----------------------------------------------------------------------------*/
gid_t do_getgid(void)
{
	return proc_info(CURRENT_TASK, GID);
}

/*----------------------------------------------------------------------------*\
 |				  do_getpgrp()				      |
\*----------------------------------------------------------------------------*/
pid_t do_getpgrp(void)
{
	return proc_info(CURRENT_TASK, PGRP);
}

/*----------------------------------------------------------------------------*\
 |				  do_getpid()				      |
\*----------------------------------------------------------------------------*/
pid_t do_getpid(void)
{
	return proc_info(CURRENT_TASK, PID);
}

/*----------------------------------------------------------------------------*\
 |				  do_getppid()				      |
\*----------------------------------------------------------------------------*/
pid_t do_getppid(void)
{
	return proc_info(CURRENT_TASK, PPID);
}

/*----------------------------------------------------------------------------*\
 |				  do_getuid()				      |
\*----------------------------------------------------------------------------*/
uid_t do_getuid(void)
{
	return proc_info(CURRENT_TASK, UID);
}

/*----------------------------------------------------------------------------*\
 |				  do_setpgrp()				      |
\*----------------------------------------------------------------------------*/
pid_t do_setpgrp(void)
{
	return proc_setpgrp();
}

/*----------------------------------------------------------------------------*\
 |				   do_alarm()				      |
\*----------------------------------------------------------------------------*/
unsigned do_alarm(unsigned seconds)
{

/* Politely ask the timer to perform the alarm() system call. */

	msg_t *msg = msg_alloc(TMR_PID, SYS_ALARM);
	msg->args.alarm.seconds = seconds;

	msg_send_receive(msg);

	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				  do_access()				      |
\*----------------------------------------------------------------------------*/
int do_access(const char *path, int amode)
{

/* Politely ask the file system to perform the access() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_ACCESS);
	msg->args.access.path = kmalloc(PATH_MAX);
	strcpy(msg->args.access.path, path);
	msg->args.access.amode = amode;

	msg_send_receive(msg);

	kfree(msg->args.access.path);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_close()				      |
\*----------------------------------------------------------------------------*/
int do_close(int fildes)
{

/* Politely ask the file system to perform the close() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_CLOSE);
	msg->args.close.fildes = fildes;

	msg_send_receive(msg);

	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				    do_dup()				      |
\*----------------------------------------------------------------------------*/
int do_dup(int fildes)
{

/* Politely ask the file system to perform the dup() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_DUP);
	msg->args.dup.fildes = fildes;

	msg_send_receive(msg);

	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_dup2()				      |
\*----------------------------------------------------------------------------*/
int do_dup2(int fildes, int fildes2)
{

/* Politely ask the file system to perform the dup2() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_DUP2);
	msg->args.dup2.fildes = fildes;
	msg->args.dup2.fildes2 = fildes2;

	msg_send_receive(msg);

	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_fcntl()				      |
\*----------------------------------------------------------------------------*/
int do_fcntl(int fildes, int cmd, int arg)
{

/* Politely ask the file system to perform the fcntl() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_FCNTL);
	msg->args.fcntl.fildes = fildes;
	msg->args.fcntl.cmd = cmd;
	msg->args.fcntl.arg = arg;

	msg_send_receive(msg);

	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_fstat()				      |
\*----------------------------------------------------------------------------*/
int do_fstat(int fildes, struct stat *buf)
{

/* Politely ask the file system to perform the fstat() system call. */

	msg_t *msg;

	if ((unsigned long) buf < pg_addr(USER, HEAP, BOTTOM))
	{
		errno = EFAULT;
		return -1;
	}

	msg = msg_alloc(FS_PID, SYS_FSTAT);
	msg->args.fstat.fildes = fildes;
	msg->args.fstat.buf = kmalloc(sizeof(struct stat));

	msg_send_receive(msg);

	buf = msg->args.fstat.buf;
	kfree(msg->args.fstat.buf);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_ioctl()				      |
\*----------------------------------------------------------------------------*/
int do_ioctl(int fildes, int request, int arg)
{

/* Politely ask the file system to perform the ioctl() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_IOCTL);
	msg->args.ioctl.fildes = fildes;
	msg->args.ioctl.request = request;
	msg->args.ioctl.arg = arg;

	msg_send_receive(msg);

	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_lseek()				      |
\*----------------------------------------------------------------------------*/
off_t do_lseek(int fildes, off_t offset, int whence)
{

/* Politely ask the file system to perform the lseek() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_LSEEK);
	msg->args.lseek.fildes = fildes;
	msg->args.lseek.offset = offset;
	msg->args.lseek.whence = whence;

	msg_send_receive(msg);

	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_open()				      |
\*----------------------------------------------------------------------------*/
int do_open(const char *path, int oflag, mode_t mode)
{

/* Politely ask the file system to perform the open() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_OPEN);
	msg->args.open.path = kmalloc(PATH_MAX);
	strcpy(msg->args.open.path, path);
	msg->args.open.oflag = oflag;
	msg->args.open.mode = mode;

	msg_send_receive(msg);

	kfree(msg->args.open.path);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_read()				      |
\*----------------------------------------------------------------------------*/
ssize_t do_read(int fildes, void *buf, size_t nbyte)
{

/* Politely ask the file system to perform the read() system call. */

	msg_t *msg;

	if ((unsigned long) buf < pg_addr(USER, HEAP, BOTTOM))
	{
		errno = EFAULT;
		return -1;
	}

	msg = msg_alloc(FS_PID, SYS_READ);
	msg->args.read.fildes = fildes;
	msg->args.read.buf = kmalloc(nbyte);
	msg->args.read.nbyte = nbyte;

	msg_send_receive(msg);

	memcpy(buf, msg->args.read.buf, nbyte);
	kfree(msg->args.read.buf);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_stat()				      |
\*----------------------------------------------------------------------------*/
int do_stat(const char *path, struct stat *buf)
{

/* Politely ask the file system to perform the stat() system call. */

	msg_t *msg;

	if ((unsigned long) buf < pg_addr(USER, HEAP, BOTTOM))
	{
		errno = EFAULT;
		return -1;
	}

	msg = msg_alloc(FS_PID, SYS_STAT);
	msg->args.stat.path = kmalloc(PATH_MAX);
	strcpy(msg->args.stat.path, path);
	msg->args.stat.buf = kmalloc(sizeof(struct stat));

	msg_send_receive(msg);

	kfree(msg->args.stat.path);
	buf = msg->args.stat.buf;
	kfree(msg->args.stat.buf);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_write()				      |
\*----------------------------------------------------------------------------*/
ssize_t do_write(int fildes, const void *buf, size_t nbyte)
{

/* Politely ask the file system to perform the write() system call. */

	msg_t *msg;

	if ((unsigned long) buf < pg_addr(USER, HEAP, BOTTOM))
	{
		errno = EFAULT;
		return -1;
	}

	msg = msg_alloc(FS_PID, SYS_WRITE);
	msg->args.write.fildes = fildes;
	msg->args.write.buf = kmalloc(nbyte);
	memcpy(msg->args.write.buf, buf, nbyte);
	msg->args.write.nbyte = nbyte;

	msg_send_receive(msg);

	kfree(msg->args.write.buf);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_chdir()				      |
\*----------------------------------------------------------------------------*/
int do_chdir(const char *path)
{

/* Politely ask the file system to perform the chdir() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_CHDIR);
	msg->args.chdir.path = kmalloc(PATH_MAX);
	strcpy(msg->args.chdir.path, path);

	msg_send_receive(msg);

	kfree(msg->args.chdir.path);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				  do_chroot()				      |
\*----------------------------------------------------------------------------*/
int do_chroot(const char *path)
{

/* Politely ask the file system to perform the chroot() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_CHROOT);
	msg->args.chroot.path = kmalloc(PATH_MAX);
	strcpy(msg->args.chroot.path, path);

	msg_send_receive(msg);

	kfree(msg->args.chroot.path);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_link()				      |
\*----------------------------------------------------------------------------*/
int do_link(const char *path1, const char *path2)
{

/* Politely ask the file system to perform the link() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_LINK);
	msg->args.link.path1 = kmalloc(PATH_MAX);
	strcpy(msg->args.link.path1, path1);
	msg->args.link.path2 = kmalloc(PATH_MAX);
	strcpy(msg->args.link.path2, path2);

	msg_send_receive(msg);

	kfree(msg->args.link.path1);
	kfree(msg->args.link.path2);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_sync()				      |
\*----------------------------------------------------------------------------*/
void do_sync(void)
{

/* Politely ask the file system to perform the sync() system call. */

	msg_send(msg_alloc(FS_PID, SYS_SYNC));

	/* Subtle: sync() doesn't return a value.  So it's kind of ridiculous to
	 * wait for a reply just to free the message's memory.  So the message's
	 * memory is freed by the file system.  I hope. */
}

/*----------------------------------------------------------------------------*\
 |				  do_unlink()				      |
\*----------------------------------------------------------------------------*/
int do_unlink(const char *path)
{

/* Politely ask the file system to perform the unlink() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_UNLINK);
	msg->args.unlink.path = kmalloc(PATH_MAX);
	strcpy(msg->args.unlink.path, path);

	msg_send_receive(msg);

	kfree(msg->args.unlink.path);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_chmod()				      |
\*----------------------------------------------------------------------------*/
int do_chmod(const char *path, mode_t mode)
{

/* Politely ask the file system to perform the chmod() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_CHMOD);
	msg->args.chmod.path = kmalloc(PATH_MAX);
	strcpy(msg->args.chmod.path, path);
	msg->args.chmod.mode = mode;

	msg_send_receive(msg);

	kfree(msg->args.chmod.path);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_chown()				      |
\*----------------------------------------------------------------------------*/
int do_chown(const char *path, uid_t owner, gid_t group)
{

/* Politely ask the file system to perform the chown() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_CHOWN);
	msg->args.chown.path = kmalloc(PATH_MAX);
	strcpy(msg->args.chown.path, path);
	msg->args.chown.owner = owner;
	msg->args.chown.group = group;

	msg_send_receive(msg);

	kfree(msg->args.chown.path);
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_umask()				      |
\*----------------------------------------------------------------------------*/
mode_t do_umask(mode_t cmask)
{

/* Politely ask the file system to perform the umask() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_UMASK);
	msg->args.umask.cmask = cmask;

	msg_send_receive(msg);

	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_time()				      |
\*----------------------------------------------------------------------------*/
time_t do_time(time_t *tloc)
{

/* Politely ask the timer to perform the time() system call. */

	msg_t *msg;

	if (tloc != NULL && (unsigned long) tloc < pg_addr(USER, HEAP, BOTTOM))
	{
		errno = EFAULT;
		return -1;
	}

	msg = msg_alloc(TMR_PID, SYS_TIME);
	msg->args.time.tloc = tloc == NULL ? NULL : kmalloc(sizeof(size_t));

	msg_send_receive(msg);

	if (tloc != NULL)
	{
		*tloc = *msg->args.time.tloc;
		kfree(msg->args.time.tloc);
	}
	return cleanup(msg);
}

/*----------------------------------------------------------------------------*\
 |				   do_utime()				      |
\*----------------------------------------------------------------------------*/
int do_utime(const char *path, const struct utimbuf *times)
{

/* Politely ask the file system to perform the utime() system call. */

	msg_t *msg = msg_alloc(FS_PID, SYS_UTIME);
	msg->args.utime.path = kmalloc(PATH_MAX);
	strcpy(msg->args.utime.path, path);
	if (times == NULL)
		msg->args.utime.times = NULL;
	else
	{
		msg->args.utime.times = kmalloc(sizeof(struct utimbuf));
		msg->args.utime.times->actime = times->actime;
		msg->args.utime.times->modtime = times->modtime;
	}

	msg_send_receive(msg);

	kfree(msg->args.utime.path);
	kfree(msg->args.utime.times);
	return cleanup(msg);
}
