/*----------------------------------------------------------------------------*\
 |	fildes.h							      |
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

#ifndef _FS_FILDES_H
#define _FS_FILDES_H

#include <fs/inode.h>
#include <sys/types.h>
#include <limits.h>
#include <config.h>

/* A file pointer is an intermediary between a file descriptor and an inode: */
typedef struct
{
	inode_t *inode_ptr;  /* Inode pointer.        */
	unsigned char count; /* Number of references. */
	off_t offset;        /* File position.        */
	int status;          /* File status.          */
	mode_t mode;         /* File mode.            */
} file_ptr_t;

/* Process-specific file system information: */
typedef struct
{
	inode_t *root_dir;                /* Root directory.            */
	inode_t *work_dir;                /* Current working directory. */
	mode_t cmask;                     /* File mode creation mask.   */
	file_ptr_t *open_descr[OPEN_MAX]; /* File descriptor table.     */
} fs_proc_t;

/* Global variables: */
file_ptr_t file_ptr[NUM_FILE_PTRS]; /* File pointer table.                 */
fs_proc_t fs_proc[NUM_PROCS];       /* Process-specific information table. */

#define OPEN_DESCR(fildes)	(fs_proc[fildes].open_descr[fildes])

/* Function prototypes: */
void descr_init(void);
int descr_get(void);
void descr_put(int fildes);
file_ptr_t *fildes_to_open_descr(int fildes);
inode_t *fildes_to_inode(int fildes);
int do_fs_dup(int fildes);
int do_fs_dup2(int fildes, int fildes2);
int do_fs_fcntl(int fildes, int cmd, int arg);
off_t do_fs_lseek(int fildes, off_t offset, int whence);

#endif
