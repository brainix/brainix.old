/*----------------------------------------------------------------------------*\
 |	mount.c								      |
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
 |				    mount()				      |
\*----------------------------------------------------------------------------*/
int mount(char *dev_name, char *mount_point_name, unsigned long flags)
{

/* Perform various checks, and mount a file system.  Return 0 on success, or a
 * negative error number. */

	inode_t *dev_inode_ptr;
	dev_t dev;
	inode_t *mount_point_inode_ptr;
	super_t *super_ptr;

	/* Check for mounting permission. */
	if (!super_user)
		/* No mounting permission. */
		return -EACCES;

	/* Resolve the device file's path name to its device number. */
	dev_inode_ptr = path_to_inode(dev_name);
	if (dev_inode_ptr == NULL || !is_blk(dev_inode_ptr))
	{
		/* The device's path name could not be resolved, or the device
		 * is not a block device. */
		inode_put(dev_inode_ptr);
		return dev_inode_ptr == NULL ? -err_code : -ENODEV;
	}
	dev = inode_to_dev(dev_inode_ptr);
	inode_put(dev_inode_ptr);

	/* Resolve the mount point's path name to an inode. */
	mount_point_inode_ptr = path_to_inode(mount_point_name);
	if (mount_point_inode_ptr == NULL || !is_dir(mount_point_inode_ptr))
	{
		/* The mount point's path name could not be resolved, or the
		 * mount point is not a directory. */
		inode_put(mount_point_inode_ptr);
		return mount_point_inode_ptr == NULL ? -err_code : -ENOTDIR;
	}

	/* Make sure the device is not already mounted, and the mount point is
	 * not already mounted on. */
	for (super_ptr = &super[0]; super_ptr < &super[NUM_SUPERS]; super_ptr++)
		if (super_ptr->dev == dev ||
		    super_ptr->mount_point_inode_ptr == mount_point_inode_ptr)
		{
			/* The device is already mounted, or the mount point is
			 * already mounted on. */
			inode_put(mount_point_inode_ptr);
			return -EBUSY;
		}

	/* Open the device. */
	dev_open_close(dev, BLOCK, OPEN);
	if (err_code)
	{
		/* The device could not be opened. */
		inode_put(mount_point_inode_ptr);
		return -err_code;
	}

	/* Read the superblock. */
	super_ptr = super_read(dev);
	if (super_ptr == NULL                      ||
	    super_ptr->s_magic != EXT2_SUPER_MAGIC ||
	    super_ptr->s_state != EXT2_VALID_FS)
	{
		/* There are too many mounted file systems, or the file system
		 * is not ext2, or the file system was uncleanly unmounted. */
		inode_put(mount_point_inode_ptr);
		dev_open_close(dev, BLOCK, CLOSE);
		return super_ptr == NULL ? -ENFILE : -EINVAL;
	}

	/* All checks passed.  Perform the mount. */

	/* Fill in the superblock's fields. */
	super_ptr->s_mtime = do_time(NULL);
	super_ptr->s_mnt_count++;
	super_ptr->s_state = EXT2_ERROR_FS;
	memcpy(super_ptr->s_last_mounted, mount_point_name,
		strlen(mount_point_name) + 1);
	super_ptr->dev = dev;
	super_ptr->block_size = 1024 << super_ptr->s_log_block_size;
	super_ptr->frag_size = super_ptr->s_log_frag_size >= 0 ?
		1024 << super_ptr->s_log_frag_size :
		1024 >> -super_ptr->s_log_frag_size;
	super_ptr->mount_point_inode_ptr = mount_point_inode_ptr;
	super_ptr->root_dir_inode_ptr = inode_get(dev, EXT2_ROOT_INO);
	super_ptr->dirty = true;

	/* Mark the mount point as mounted on. */
	mount_point_inode_ptr->mounted = true;

	return 0;
}

/*----------------------------------------------------------------------------*\
 |				  mount_root()				      |
\*----------------------------------------------------------------------------*/
void mount_root(void)
{

/* Mount the root file system. */

	super_t *super_ptr;

	/* Open the device. */
	dev_open_close(ROOT_DEV, BLOCK, OPEN);
	if (err_code)
		/* The device could not be opened. */
		panic("mount_root", strerror(err_code));

	/* Read the superblock. */
	super_ptr = super_read(ROOT_DEV);
	if (err_code)
		/* The superblock could not be read. */
		panic("mount_root", strerror(err_code));

	/* Perform the mount.  Fill in the superblock's fields. */
	super_ptr->s_mtime = do_time(NULL);
	super_ptr->s_mnt_count++;
	super_ptr->s_state = EXT2_ERROR_FS;
	memcpy(super_ptr->s_last_mounted, "/\0", 2);
	super_ptr->dev = ROOT_DEV;
	super_ptr->block_size = 1024 << super_ptr->s_log_block_size;
	super_ptr->frag_size = super_ptr->s_log_frag_size >= 0 ?
		1024 << super_ptr->s_log_frag_size :
		1024 >> -super_ptr->s_log_frag_size;
	super_ptr->mount_point_inode_ptr = NULL;
	super_ptr->root_dir_inode_ptr = inode_get(ROOT_DEV, EXT2_ROOT_INO);
	super_ptr->dirty = true;
/**/
}

/*----------------------------------------------------------------------------*\
 |				    umount()				      |
\*----------------------------------------------------------------------------*/
int umount(char *dev_name)
{

/* Perform various checks, and unmount a file system.  Return 0 on success, or a
 * negative error number. */

	inode_t *dev_inode_ptr;
	dev_t dev;
	super_t *super_ptr;
	inode_t *inode_ptr;
	unsigned char count = 0;

	/* Check for mounting permission. */
	if (!super_user)
		/* No mounting permission. */
		return -EACCES;

	/* Resolve the device file's path name to its device number. */
	dev_inode_ptr = path_to_inode(dev_name);
	if (dev_inode_ptr == NULL || !is_blk(dev_inode_ptr))
	{
		/* The device's path name could not be resolved, or the device
		 * is not a block device. */
		inode_put(dev_inode_ptr);
		return dev_inode_ptr == NULL ? -err_code : -ENODEV;
	}
	dev = inode_to_dev(dev_inode_ptr);
	inode_put(dev_inode_ptr);

	/* Get the superblock. */
	super_ptr = super_get(dev);
	if (super_ptr == NULL)
		/* The device is not mounted. */
		return -EINVAL;

	/* Make sure the file system is not busy. */
	for (inode_ptr = &inode[0]; inode_ptr < &inode[NUM_INODES]; inode_ptr++)
		if (inode_ptr->dev == dev && inode_ptr->count > 0)
			if ((count += inode_ptr->count) > 1)
				/* The file system is busy. */
				return -EBUSY;

	/* All checks passed.  Perform the unmount. */

	/* Mark the file system as cleanly unmounted. */
	super_ptr->s_state = EXT2_VALID_FS;
	super_ptr->dirty = true;
	super_write(super_ptr);

	/* Release the mount point and root directory inodes, flush all buffered
	 * data, and close the device. */
	inode_put(super_ptr->mount_point_inode_ptr);
	inode_put(super_ptr->root_dir_inode_ptr);
	dev_sync(dev);
	dev_purge(dev);
	dev_open_close(dev, BLOCK, CLOSE);

	/* Reinitialize the slot in the superblock table for future use. */
	super_ptr->dev = NO_DEV;
	super_ptr->block_size = 0;
	super_ptr->frag_size = 0;
	super_ptr->mount_point_inode_ptr = NULL;
	super_ptr->root_dir_inode_ptr = NULL;
	super_ptr->dirty = false;

	return 0;
}

/*----------------------------------------------------------------------------*\
 |				 umount_root()				      |
\*----------------------------------------------------------------------------*/
void umount_root(void)
{

/* Unmount the root file system. */

	super_t *super_ptr = super_get(ROOT_DEV);

	/* Mark the file system as cleanly unmounted. */
	super_ptr->s_state = EXT2_VALID_FS;
	super_ptr->dirty = true;
	super_write(super_ptr);

	/* Release the mount point and root directory inodes, flush all buffered
	 * data, and close the device. */
	inode_put(super_ptr->mount_point_inode_ptr);
	inode_put(super_ptr->root_dir_inode_ptr);
	dev_sync(ROOT_DEV);
	dev_purge(ROOT_DEV);
	dev_open_close(ROOT_DEV, BLOCK, CLOSE);

	/* Reinitialize the slot in the superblock table for future use. */
	super_ptr->dev = NO_DEV;
	super_ptr->block_size = 0;
	super_ptr->frag_size = 0;
	super_ptr->mount_point_inode_ptr = NULL;
	super_ptr->root_dir_inode_ptr = NULL;
	super_ptr->dirty = false;
}
