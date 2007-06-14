/*----------------------------------------------------------------------------*\
 |	message.h							      |
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

#ifndef _KERNEL_MESSAGE_H
#define _KERNEL_MESSAGE_H

typedef long mid_t;

typedef struct
{
	ptrdiff_t increment;
	void *ret_val;
} sbrk_args_t;

typedef struct
{
	pid_t ret_val;
} fork_args_t;

typedef struct
{
	char *path;
	char **argv;
	char **envp;
	int ret_val;
} execve_args_t;

typedef struct
{
	int status;
} exit_args_t;

typedef struct
{
	pid_t ret_val;
} getpgrp_args_t;

typedef struct
{
	pid_t ret_val;
} getpid_args_t;

typedef struct
{
	pid_t ret_val;
} setsid_args_t;

typedef struct
{
	int *stat_loc;
	pid_t ret_val;
} wait_args_t;

typedef struct
{
	pid_t pid;
	int *stat_loc;
	int options;
	pid_t ret_val;
} waitpid_args_t;

typedef struct
{
	unsigned seconds;
	unsigned ret_val;
} alarm_args_t;

typedef struct
{
	char *path;
	int amode;
	int ret_val;
} access_args_t;

typedef struct
{
	int fildes;
	int ret_val;
} close_args_t;

typedef struct
{
	char *path;
	mode_t mode;
	int ret_val;
} creat_args_t;

typedef struct
{
	int fildes;
	int ret_val;
} dup_args_t;

typedef struct
{
	int fildes;
	int fildes2;
	int ret_val;
} dup2_args_t;

typedef struct
{
	int fildes;
	int cmd;
	int arg;
	int ret_val;
} fcntl_args_t;

typedef struct
{
	int fildes;
	struct stat *buf;
	int ret_val;
} fstat_args_t;

typedef struct
{
	int fildes;
	int request;
	int arg;
	int ret_val;
} ioctl_args_t;

typedef struct
{
	int fildes;
	off_t offset;
	int whence;
	off_t ret_val;
} lseek_args_t;

typedef struct
{
	char *path;
	mode_t mode;
	dev_t dev;
	int ret_val;
} mknod_args_t;

typedef struct
{
	char *path;
	int oflag;
	mode_t mode;
	int ret_val;
} open_args_t;

typedef struct
{
	int fildes[2];
	int ret_val;
} pipe_args_t;

typedef struct
{
	int fildes;
	void *buf;
	size_t nbyte;
	ssize_t ret_val;
} read_args_t;

typedef struct
{
	char *old;
	char *new;
	int ret_val;
} rename_args_t;

typedef struct
{
	char *path;
	struct stat *buf;
	int ret_val;
} stat_args_t;

typedef struct
{
	int fildes;
	void *buf;
	size_t nbyte;
	ssize_t ret_val;
} write_args_t;

typedef struct
{
	char *path;
	int ret_val;
} chdir_args_t;

typedef struct
{
	char *path;
	int ret_val;
} chroot_args_t;

typedef struct
{
	char *path1;
	char *path2;
	int ret_val;
} link_args_t;

typedef struct
{
	char *path;
	mode_t mode;
	int ret_val;
} mkdir_args_t;

typedef struct
{
	char *path;
	int ret_val;
} rmdir_args_t;

typedef struct
{
	char *path;
	int ret_val;
} unlink_args_t;

typedef struct
{
	char *path;
	mode_t mode;
	int ret_val;
} chmod_args_t;

typedef struct
{
	char *path;
	uid_t owner;
	gid_t group;
	int ret_val;
} chown_args_t;

typedef struct
{
	gid_t ret_val;
} getgid_args_t;

typedef struct
{
	uid_t ret_val;
} getuid_args_t;

typedef struct
{
	gid_t gid;
	int ret_val;
} setgid_args_t;

typedef struct
{
	uid_t uid;
	int ret_val;
} setuid_args_t;

typedef struct
{
	mode_t cmask;
	mode_t ret_val;
} umask_args_t;

typedef struct
{
	time_t *tloc;
	time_t ret_val;
} time_args_t;

typedef struct
{
	struct tms *buffer;
	clock_t ret_val;
} times_args_t;

typedef struct
{
	char *path;
	struct utimbuf *times;
	int ret_val;
} utime_args_t;

typedef struct
{
	unsigned char irq;
	bool block;
	unsigned char maj;
} brnx_reg_args_t;

typedef struct
{
	unsigned char scode;
} brnx_irq_args_t;

typedef struct
{
	unsigned char counter;
	unsigned short freq;
} brnx_freq_args_t;

typedef struct
{
	clock_t ticks;
	unsigned char type;
} brnx_watch_args_t;

typedef struct
{
	clock_t ret_val;
} brnx_uptime_args_t;

typedef struct
{
	bool reboot;
} brnx_shutdown_args_t;

typedef struct
{
	pid_t pid;
	unsigned long entry;
} exec_args_t;

typedef struct
{
	unsigned char min;
	int ret_val;
} open_close_args_t;

typedef struct
{
	unsigned char min;
	off_t off;
	size_t size;
	void *buf;
	ssize_t ret_val;
} read_write_args_t;

typedef struct msg
{
	mid_t mid;
	pid_t from;
	pid_t to;
	unsigned char op;

	union
	{
		sbrk_args_t sbrk;
		fork_args_t fork;
		execve_args_t execve;
		exit_args_t exit;
		getpgrp_args_t getpgrp;
		getpid_args_t getpid;
		setsid_args_t setsid;
		wait_args_t wait;
		waitpid_args_t waitpid;

		alarm_args_t alarm;

		access_args_t access;
		close_args_t close;
		creat_args_t creat;
		dup_args_t dup;
		dup2_args_t dup2;
		fcntl_args_t fcntl;
		fstat_args_t fstat;
		ioctl_args_t ioctl;
		lseek_args_t lseek;
		mknod_args_t mknod;
		open_args_t open;
		pipe_args_t pipe;
		read_args_t read;
		rename_args_t rename;
		stat_args_t stat;
		write_args_t write;

		chdir_args_t chdir;
		chroot_args_t chroot;
		link_args_t link;
		mkdir_args_t mkdir;
		rmdir_args_t rmdir;
		unlink_args_t unlink;

		chmod_args_t chmod;
		chown_args_t chown;
		getgid_args_t getgid;
		getuid_args_t getuid;
		setgid_args_t setgid;
		setuid_args_t setuid;
		umask_args_t umask;

		time_args_t time;
		times_args_t times;
		utime_args_t utime;

		brnx_reg_args_t brnx_reg;
		brnx_irq_args_t brnx_irq;
		brnx_freq_args_t brnx_freq;
		brnx_watch_args_t brnx_watch;
		brnx_uptime_args_t brnx_uptime;
		brnx_shutdown_args_t brnx_shutdown;
		exec_args_t exec;
		open_close_args_t open_close;
		read_write_args_t read_write;
	} args;

	struct msg *prev;
	struct msg *next;
} msg_t;

#endif
