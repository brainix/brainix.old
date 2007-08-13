/*----------------------------------------------------------------------------*\
 |	device.c							      |
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

#include <fs/fs.h>

/*----------------------------------------------------------------------------*\
 |				dev_to_maj_min()			      |
\*----------------------------------------------------------------------------*/
void dev_to_maj_min(dev_t dev, unsigned char *maj, unsigned char *min)
{

/* Extract the minor number and the minor number from a device number. */

	*maj = (dev & 0xFF00) >> 8;
	*min = (dev & 0x00FF) >> 0;
}

/*----------------------------------------------------------------------------*\
 |				maj_min_to_dev()			      |
\*----------------------------------------------------------------------------*/
dev_t maj_min_to_dev(unsigned char maj, unsigned char min)
{

/* Build the device number from a major number and a minor number. */

	return ((maj & 0xFF) << 8) | ((min & 0xFF) << 0);
}

/*----------------------------------------------------------------------------*\
 |				   dev_init()				      |
\*----------------------------------------------------------------------------*/
void dev_init(void)
{

/* Initialize the device driver PID table. */

	unsigned char maj;

	for (maj = 0; maj < NUM_DRIVERS; maj++)
		driver_pid[BLOCK][maj] =
		driver_pid[CHAR][maj] = NO_PID;
}

/*----------------------------------------------------------------------------*\
 |				 fs_register()				      |
\*----------------------------------------------------------------------------*/
void fs_register(bool block, unsigned char maj, pid_t pid)
{

/*
 | Register a device driver with the file system - map a device's major number
 | to its driver's PID.  If the driver for the device containing the root file
 | system is being registered, mount the root file system and initialize the
 | root and current working directories.
 */

	dev_t dev;

	/* Register the device driver with the file system. */
	driver_pid[block][maj] = pid;

	if (block && maj == ROOT_MAJ)
	{
		/*
		 | The driver for the device containing the root file system is
		 | being registered.
		 */
		mount_root();
		dev = maj_min_to_dev(ROOT_MAJ, ROOT_MIN);
		fs_proc[FS_PID].root_dir = inode_get(dev, EXT2_ROOT_INO);
		fs_proc[FS_PID].work_dir = inode_get(dev, EXT2_ROOT_INO);
	}
}

/*----------------------------------------------------------------------------*\
 |				  dev_deinit()				      |
\*----------------------------------------------------------------------------*/
void dev_deinit(void)
{
	unsigned char maj;
	unsigned char block;
	pid_t pid;

	for (maj = 0; maj < NUM_DRIVERS; maj++)
		for (block = 0; block <= 1; block++)
			if ((pid = driver_pid[block][maj]) != NO_PID)
			{
				msg_send(msg_alloc(pid, SHUTDOWN));
				msg_free(msg_receive(pid));
			}
}

/*----------------------------------------------------------------------------*\
 |				 inode_to_dev()				      |
\*----------------------------------------------------------------------------*/
dev_t inode_to_dev(inode_t *inode_ptr)
{

/* Extract the device number from an inode. */

	dev_t dev = NO_DEV;

	if (is_blk(inode_ptr) || is_chr(inode_ptr))
		dev = inode_ptr->i_block[0];
	return dev;
}

/*----------------------------------------------------------------------------*\
 |				 inode_to_pid()				      |
\*----------------------------------------------------------------------------*/
pid_t inode_to_pid(inode_t *inode_ptr)
{
	unsigned char maj, min;
	pid_t pid = NO_PID;

	if (is_blk(inode_ptr) || is_chr(inode_ptr))
	{
		dev_to_maj_min(inode_to_dev(inode_ptr), &maj, &min);
		pid = driver_pid[is_blk(inode_ptr)][maj];
	}
	return pid;
}

/*----------------------------------------------------------------------------*\
 |				dev_open_close()			      |
\*----------------------------------------------------------------------------*/
int dev_open_close(dev_t dev, bool block, bool open)
{

/* If open is true, open a device.  Otherwise, close a device. */

	unsigned char maj, min;
	pid_t pid;
	msg_t *m;
	int ret_val;

	/* Find the device driver's PID. */
	dev_to_maj_min(dev, &maj, &min);
	pid = driver_pid[block][maj];
	if (pid == NO_PID)
		return -(err_code = ENXIO);

	/* Send a message to the device driver. */
	m = msg_alloc(pid, open ? SYS_OPEN : SYS_CLOSE);
	m->args.open_close.min = min;
	msg_send(m);

	/* Await the device driver's reply. */
	m = msg_receive(pid);
	ret_val = m->args.open_close.ret_val;
	msg_free(m);
	if (ret_val < 0)
		err_code = -ret_val;
	return ret_val;
}

/*----------------------------------------------------------------------------*\
 |				    dev_rw()				      |
\*----------------------------------------------------------------------------*/
ssize_t dev_rw(dev_t dev, bool block, bool read, off_t off, size_t size,
	void *buf)
{

/* If read is true, read from a device.  Otherwise, write to a device. */

	unsigned char maj, min;
	pid_t pid;
	msg_t *m;
	ssize_t ret_val;

	/* Find the device driver's PID. */
	dev_to_maj_min(dev, &maj, &min);
	pid = driver_pid[block][maj];
	if (pid == NO_PID)
		return -(err_code = ENXIO);

	/* Send a message to the device driver. */
	m = msg_alloc(pid, read ? SYS_READ : SYS_WRITE);
	m->args.read_write.min = min;
	m->args.read_write.off = off;
	m->args.read_write.size = size;
	m->args.read_write.buf = buf;
	msg_send(m);

	/* Await the device driver's reply. */
	m = msg_receive(pid);
	ret_val = m->args.read_write.ret_val;
	msg_free(m);
	if (ret_val < 0)
		err_code = -ret_val;
	return ret_val;
}

/*----------------------------------------------------------------------------*\
 |				 do_fs_ioctl()				      |
\*----------------------------------------------------------------------------*/
int do_fs_ioctl(int fildes, int request, int arg)
{
	file_ptr_t *open_descr;
	inode_t *inode_ptr;
	dev_t dev;
	unsigned char maj, min;
	pid_t pid;
	msg_t *m;
	int ret_val;

	/* Find the device driver's PID. */
	open_descr = fildes_to_open_descr(fildes);
	if (open_descr == NULL)
		return -(err_code = EBADF);
	inode_ptr = open_descr->inode_ptr;
	dev = inode_to_dev(inode_ptr);
	if (dev == NO_DEV)
		return -(err_code = ENOTTY);
	dev_to_maj_min(dev, &maj, &min);
	pid = driver_pid[is_blk(inode_ptr)][maj];
	if (pid == NO_PID)
		return -(err_code = ENXIO);

	/* Send a message to the device driver. */
	m = msg_alloc(pid, SYS_IOCTL);
	m->args.ioctl.fildes = min;
	m->args.ioctl.request = request;
	m->args.ioctl.arg = arg;
	msg_send(m);

	/* Await the device driver's reply. */
	m = msg_receive(pid);
	ret_val = m->args.ioctl.ret_val;
	msg_free(m);
	if (ret_val < 0)
		err_code = ret_val;
	return ret_val;
}
