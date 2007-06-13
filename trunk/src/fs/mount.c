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
	debug(-FS_ESOTERIC+1,"Attempting to mount a file system.");
/* Perform various checks, and mount a file system.  Return 0 on success, or a
 * negative error number. */

	inode_t *dev_inode_ptr;
	dev_t dev;
	inode_t *mount_point_inode_ptr;
	super_t *super_ptr;
	debug(-FS_ESOTERIC+2,"filesystem.mount(): Check for mounting permission. \n");
	
	/* Check for mounting permission. */
	if (!super_user)
	{
		debug(-FS_ESOTERIC+2,"filesystem.mount(): No mounting permission. \n");

		/* No mounting permission. */
		return -EACCES;
	}


	debug(-FS_ESOTERIC+1,"filesystem.mount(): Resolve the device file's path name to its device number. \n");

	/* Resolve the device file's path name to its device number. */
	dev_inode_ptr = path_to_inode(dev_name);
	if (dev_inode_ptr == NULL || !is_blk(dev_inode_ptr))
	{

		debug(-FS_ESOTERIC+2,"filesystem.mount(): Path could not be resolved or invalid device.\n");

		/* The device's path name could not be resolved, or the device
		 * is not a block device. */
		inode_put(dev_inode_ptr);
		return dev_inode_ptr == NULL ? -err_code : -ENODEV;
	}
	dev = inode_to_dev(dev_inode_ptr);
	inode_put(dev_inode_ptr);
	

	debug(-FS_ESOTERIC+1,"filesystem.mount(): Resolve the mount point's path name to an inode.\n");

	/* Resolve the mount point's path name to an inode. */
	mount_point_inode_ptr = path_to_inode(mount_point_name);
	if (mount_point_inode_ptr == NULL || !is_dir(mount_point_inode_ptr))
	{

		debug(-FS_ESOTERIC+2,"filesystem.mount(): Pathname could not be resolved or the inode is not a directory.\n");

		/* The mount point's path name could not be resolved, or the
		 * mount point is not a directory. */
		inode_put(mount_point_inode_ptr);
		return mount_point_inode_ptr == NULL ? -err_code : -ENOTDIR;
	}


	debug(-FS_ESOTERIC+1,"filesystem.mount(): Make sure the device is not mounted and the mount point is not already mounted on.\n");

	/* Make sure the device is not already mounted, and the mount point is
	 * not already mounted on. */
	for (super_ptr = &super[0]; super_ptr < &super[NUM_SUPERS]; super_ptr++)
		if (super_ptr->dev == dev ||
		    super_ptr->mount_point_inode_ptr == mount_point_inode_ptr)
		{

			debug(-FS_ESOTERIC+3,"filesystem.mount(): The device is already mounted or the mount point is.\n");

			/* The device is already mounted, or the mount point is
			 * already mounted on. */
			inode_put(mount_point_inode_ptr);
			return -EBUSY;
		}

	debug(-FS_ESOTERIC+1,"filesystem.mount(): opening the device \n");

	/* Open the device. */
	dev_open_close(dev, BLOCK, OPEN);
	if (err_code)
	{
		/* The device could not be opened. */
		inode_put(mount_point_inode_ptr);

		debug(-FS_ESOTERIC+2,"filesystem.mount(): device could not be opened \n");

		return -err_code;
	}

	debug(-FS_ESOTERIC+1,"filesystem.mount(): the device was opened successfully \n");
	debug(-FS_ESOTERIC+1,"filesystem.mount(): reading the superblock \n");

	/* Read the superblock. */
	super_ptr = super_read(dev);
	if (super_ptr == NULL                      ||
	    super_ptr->s_magic != EXT2_SUPER_MAGIC ||
	    super_ptr->s_state != EXT2_VALID_FS)
	{

		debug(-FS_ESOTERIC+2,"filesystem.mount(): Error: too many mounted filesystems, invalid system type, or system was unclearly mounted \n");

		/* There are too many mounted file systems, or the file system
		 * is not ext2, or the file system was uncleanly unmounted. */
		inode_put(mount_point_inode_ptr);
		dev_open_close(dev, BLOCK, CLOSE);
		return super_ptr == NULL ? -ENFILE : -EINVAL;
	}

	/* All checks passed.  Perform the mount. */

	debug(-FS_ESOTERIC+1,"filesystem.mount(): All checks passed.  Perform the mount. \n");

	/* Fill in the superblock's fields. */

	debug(-FS_ESOTERIC+1,"filesystem.mount(): Filling in the superblock's fields. \n");

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


	debug(-FS_ESOTERIC+1,"filesystem.mount(): Marking the mount points. \n");

	/* Mark the mount point as mounted on. */
	mount_point_inode_ptr->mounted = true;

	debug(-FS_ESOTERIC+1,"filesystem.mount(): Done. \n");

	return 0;
}

/*----------------------------------------------------------------------------*\
 |				  mount_root()				      |
\*----------------------------------------------------------------------------*/
void mount_root(void)
{

/* Mount the root file system. */

	debug(-FS_ESOTERIC+1,"FileSystem.mount_root(): Mount the root file system.\n");

	super_t *super_ptr;

	/* Open the device. */

	debug(-FS_ESOTERIC+2,"FileSystem.mount_root(): Open the device.\n");

	dev_open_close(ROOT_DEV, BLOCK, OPEN);
	if (err_code)
	{

		debug(-FS_ESOTERIC+3,"FileSystem.mount_root(): Device Could Not Be Opened!\n");

		/* The device could not be opened. */
		panic("mount_root", strerror(err_code));
	}

	/* Read the superblock. */

	debug(-FS_ESOTERIC+2,"FileSystem.mount_root(): Read the super block.\n");

	super_ptr = super_read(ROOT_DEV);
	if (err_code)
	{

		debug(-FS_ESOTERIC+3,"FileSystem.mount_root(): The Superblock could not be read!\n");

		/* The superblock could not be read. */
		panic("mount_root", strerror(err_code));
	}

	debug(-FS_ESOTERIC+2,"FileSystem.mount_root(): Performing the mount..\n");

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

	debug(-FS_ESOTERIC+2, "filesystem.umount(): Check for mounting permission.\n");
	/* Check for mounting permission. */
	if (!super_user)
	{
		debug(-FS_ESOTERIC+3, "filesystem.umount(): No mounting permissions.\n");
		/* No mounting permission. */
		return -EACCES;
	}

	debug(-FS_ESOTERIC+2, "filesystem.umount(): Resolve the device file's path name to its device number.\n");
	/* Resolve the device file's path name to its device number. */
	dev_inode_ptr = path_to_inode(dev_name);
	if (dev_inode_ptr == NULL || !is_blk(dev_inode_ptr))
	{
		debug(-FS_ESOTERIC+4, "filesystem.umount(): The device's path name could not be resolved, or the device is not a block device.\n");
		/* The device's path name could not be resolved, or the device
		 * is not a block device. */
		inode_put(dev_inode_ptr);
		return dev_inode_ptr == NULL ? -err_code : -ENODEV;
	}
	dev = inode_to_dev(dev_inode_ptr);
	inode_put(dev_inode_ptr);

	debug(-FS_ESOTERIC+2, "filesystem.umount(): Get the superblock.\n");
	/* Get the superblock. */
	super_ptr = super_get(dev);
	if (super_ptr == NULL)
	{
		debug(-FS_ESOTERIC+3, "filesystem.umount(): The device is not mounted.\n");
		/* The device is not mounted. */
		return -EINVAL;
	}

	debug(-FS_ESOTERIC+2, "filesystem.umount(): Make sure the file system is not busy.\n");
	/* Make sure the file system is not busy. */
	for (inode_ptr = &inode[0]; inode_ptr < &inode[NUM_INODES]; inode_ptr++)
		if (inode_ptr->dev == dev && inode_ptr->count > 0)
			if ((count += inode_ptr->count) > 1)
			{
				debug(-FS_ESOTERIC+2, "filesystem.umount(): The file system is busy.\n");
				/* The file system is busy. */
				return -EBUSY;
			}

	debug(-FS_ESOTERIC+3, "filesystem.umount(): All checks passed. Perform the unmount.\n");
	/* All checks passed.  Perform the unmount. */

	debug(-FS_ESOTERIC+4, "filesystem.umount(): Mark the file system as cleanly unmounted.\n");
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
