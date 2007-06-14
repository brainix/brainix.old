/*----------------------------------------------------------------------------*\
 |	fildes.c							      |
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

#include <fs/fs.h>

/*----------------------------------------------------------------------------*\
 |				  descr_init()				      |
\*----------------------------------------------------------------------------*/
void descr_init(void)
{

/* Initialize the file pointer table and the process-specific file system
 * information table. */

	int ptr_index;
	pid_t pid;
	int descr_index;

	/* Initialize the file pointer table. */
	for (ptr_index = 0; ptr_index < NUM_FILE_PTRS; ptr_index++)
	{
		file_ptr[ptr_index].inode_ptr = NULL;
		file_ptr[ptr_index].count = 0;
		file_ptr[ptr_index].offset = 0;
		file_ptr[ptr_index].status = 0;
		file_ptr[ptr_index].mode = 0;
	}

	/* Initialize the process-specific file system information table. */
	for (pid = 0; pid < NUM_PROCS; pid++)
	{
		fs_proc[pid].root_dir = NULL;
		fs_proc[pid].work_dir = NULL;
		fs_proc[pid].cmask = 0;
		for (descr_index = 0; descr_index < OPEN_MAX; descr_index++)
			fs_proc[pid].open_descr[descr_index] = NULL;
	}
}

/*----------------------------------------------------------------------------*\
 |				  descr_get()				      |
\*----------------------------------------------------------------------------*/
int descr_get(void)
{
	int fildes;
	int ptr_index;

	for (fildes = 0; fildes < OPEN_MAX; fildes++)
		if (OPEN_DESCR(fildes) == NULL)
			break;
	if (fildes == OPEN_MAX)
		/* Too many open files in process. */
		return -(err_code = EMFILE);

	for (ptr_index = 0; ptr_index < NUM_FILE_PTRS; ptr_index++)
		if (file_ptr[ptr_index].count == 0)
			break;
	if (ptr_index == NUM_FILE_PTRS)
		/* Too many open files in system. */
		return -(err_code = ENFILE);

	OPEN_DESCR(fildes) = &file_ptr[ptr_index];
	return fildes;
}

/*----------------------------------------------------------------------------*\
 |				  descr_put()				      |
\*----------------------------------------------------------------------------*/
void descr_put(int fildes)
{
	if (fildes < 0 || fildes >= OPEN_MAX)
		return;
	OPEN_DESCR(fildes) = NULL;
}

/*----------------------------------------------------------------------------*\
 |			     fildes_to_open_descr()			      |
\*----------------------------------------------------------------------------*/
file_ptr_t *fildes_to_open_descr(int fildes)
{
	if (fildes < 0 || fildes >= OPEN_MAX)
		return NULL;
	return OPEN_DESCR(fildes);
}

/*----------------------------------------------------------------------------*\
 |			       fildes_to_inode()			      |
\*----------------------------------------------------------------------------*/
inode_t *fildes_to_inode(int fildes)
{
	if (fildes < 0 || fildes >= OPEN_MAX)
		return NULL;
	return OPEN_DESCR(fildes)->inode_ptr;
}

/*----------------------------------------------------------------------------*\
 |				  do_fs_dup()				      |
\*----------------------------------------------------------------------------*/
int do_fs_dup(int fildes)
{
	return do_fs_fcntl(fildes, F_DUPFD, 0);
}

/*----------------------------------------------------------------------------*\
 |				  do_fs_dup2()				      |
\*----------------------------------------------------------------------------*/
int do_fs_dup2(int fildes, int fildes2)
{
	if (fildes2 < 0 || fildes2 >= OPEN_MAX)
		return -(err_code = EBADF);
	if (fildes < 0 || fildes >= OPEN_MAX)
		return -(err_code = EBADF);
	if (OPEN_DESCR(fildes) != NULL)
		if (OPEN_DESCR(fildes) == OPEN_DESCR(fildes2))
			return fildes2;

	do_fs_close(fildes2);
	return do_fs_fcntl(fildes, F_DUPFD, fildes2);
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_fcntl()				      |
\*----------------------------------------------------------------------------*/
int do_fs_fcntl(int fildes, int cmd, int arg)
{
	file_ptr_t *open_descr = fildes_to_open_descr(fildes);

	if (open_descr == NULL)
		return -(err_code = EBADF);

	if (cmd == F_DUPFD)
	{
		/* Duplicate file descriptor. */
		if (arg == 0)
			while (OPEN_DESCR(arg) != NULL)
				if (++arg == OPEN_MAX)
					return -(err_code = EMFILE);
		OPEN_DESCR(arg) = OPEN_DESCR(fildes);
		OPEN_DESCR(arg)->count++;
		return arg;
	}

	if (cmd == F_GETFD)
	{
		/* Get file descriptor flags. */
	}

	if (cmd == F_SETFD)
	{
		/* Set file descriptor flags. */
	}

	if (cmd == F_GETFL)
	{
		/* Get file status flags and file access modes. */
	}

	if (cmd == F_SETFL)
	{
		/* Set file status flags. */
	}

	if (cmd == F_GETLK)
	{
		/* Get record locking information. */
	}

	if (cmd == F_SETLK)
	{
		/* Set record locking information. */
	}

	if (cmd == F_SETLKW)
	{
		/* Set record locking information; wait if blocked. */
	}

	if (cmd == F_GETOWN)
	{
		/* Get process or process group ID to receive SIGURG signals. */
	}

	if (cmd == F_SETOWN)
	{
		/* Set process or process group ID to receive SIGURG signals. */
	}

	return -(err_code = EINVAL);
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_lseek()				      |
\*----------------------------------------------------------------------------*/
off_t do_fs_lseek(int fildes, off_t offset, int whence)
{
	file_ptr_t *open_descr = fildes_to_open_descr(fildes);
	off_t start;

	if (open_descr == NULL)
		return -(err_code = EBADF);

	switch (whence)
	{
		case SEEK_CUR: start = open_descr->offset;            break;
		case SEEK_END: start = open_descr->inode_ptr->i_size; break;
		case SEEK_SET: start = 0;                             break;
		default:       return -(err_code = EINVAL);           break;
	}
	return open_descr->offset = start + offset;
}
