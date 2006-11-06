/*----------------------------------------------------------------------------*\
 |	proc.c								      |
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

#include <fs/fs.h>

/*----------------------------------------------------------------------------*\
 |				  do_fs_fork()				      |
\*----------------------------------------------------------------------------*/
void do_fs_fork(pid_t parent, pid_t child)
{
	int fildes;

	(fs_proc[child].root_dir = fs_proc[parent].root_dir)->count++;
	(fs_proc[child].work_dir = fs_proc[parent].work_dir)->count++;
	fs_proc[child].cmask = fs_proc[parent].cmask;
	for (fildes = 0; fildes < OPEN_MAX; fildes++)
		if ((fs_proc[child].open_descr[fildes] =
			fs_proc[parent].open_descr[fildes]) != NULL)
			fs_proc[child].open_descr[fildes]->count++;
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_execve()				      |
\*----------------------------------------------------------------------------*/
int do_fs_execve(const char *path, char *const argv[], char *const envp[])
{
	int fildes;
	off_t size;
	char *image = NULL;
	Elf32_Addr entry;
	msg_t *m;

	fildes = do_fs_open(path, O_RDONLY, 0);
	if (err_code)
	{
		do_fs_close(fildes);
		do_free(image);
		return -err_code;
	}

	size = do_fs_lseek(fildes, 0, SEEK_END);
	image = do_malloc(size);
	if (image == NULL)
	{
		do_fs_close(fildes);
		do_free(image);
		return -(err_code = ENOMEM);
	}

	do_fs_lseek(fildes, 0, SEEK_SET);
	do_fs_read(fildes, image, size);
	if (err_code)
	{
		do_fs_close(fildes);
		do_free(image);
		return -err_code;
	}

	elf_load(image, &entry);
	if (err_code)
	{
		do_fs_close(fildes);
		do_free(image);
		return -err_code;
	}

	do_fs_close(fildes);

	m = msg_alloc(KERNEL_PID, SYS_EXECVE);
	m->args.exec.pid = msg->from;
	m->args.exec.entry = entry;
	msg_send(m);

	return 0;
}

/*----------------------------------------------------------------------------*\
 |				  do_fs_exit()				      |
\*----------------------------------------------------------------------------*/
void do_fs_exit(int status)
{
	int fildes;

	inode_put(fs_proc[msg->from].root_dir);
	fs_proc[msg->from].root_dir = NULL;
	inode_put(fs_proc[msg->from].work_dir);
	fs_proc[msg->from].work_dir = NULL;
	for (fildes = 0; fildes < OPEN_MAX; fildes++)
		if (fs_proc[msg->from].open_descr[fildes] != NULL)
			do_fs_close(fildes);
}
