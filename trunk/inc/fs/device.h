/*----------------------------------------------------------------------------*\
 |	device.h							      |
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

#ifndef _FS_DEVICE_H
#define _FS_DEVICE_H

#include <config.h>
#include <sys/types.h>
#include <fs/inode.h>

/* Used to represent no PID: */
#define NO_PID		-1

/* Used to represent no device: */
#define NO_DEV		0

/* Device containing the root file system: */
#define ROOT_DEV	maj_min_to_dev(ROOT_MAJ, ROOT_MIN)

/* The device driver PID table maps major numbers to PIDs: */
pid_t driver_pid[2][NUM_DRIVERS];

/* Function prototypes: */
void dev_to_maj_min(dev_t dev, unsigned char *maj, unsigned char *min);
dev_t maj_min_to_dev(unsigned char maj, unsigned char min);
void dev_init(void);
void fs_register(bool block, unsigned char maj, pid_t pid);
void dev_deinit(void);
dev_t inode_to_dev(inode_t *inode_ptr);
pid_t inode_to_pid(inode_t *inode_ptr);
int dev_open_close(dev_t dev, bool block, bool open);
ssize_t dev_rw(dev_t dev, bool block, bool read, off_t off, size_t size,
	void *buf);
int do_fs_ioctl(int fildes, int request, int arg);

#endif
