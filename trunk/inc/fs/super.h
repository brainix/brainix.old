/*----------------------------------------------------------------------------*\
 |	super.h								      |
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

#ifndef _FS_SUPER_H
#define _FS_SUPER_H

#include <sys/types.h>
#include <fs/inode.h>
#include <stdbool.h>
#include <config.h>

/* The superblock describes the configuration of the file system: */
typedef struct
{
	/* The following fields reside on the device: */
	unsigned long s_inodes_count;      /* Total number of inodes.         */
	unsigned long s_blocks_count;      /* Total number of blocks.         */
	unsigned long s_r_blocks_count;    /* Number of reserved blocks.      */
	unsigned long s_free_blocks_count; /* Number of free blocks.          */
	unsigned long s_free_inodes_count; /* Number of free inodes.          */
	unsigned long s_first_data_block;  /* Block containing superblock.    */
	unsigned long s_log_block_size;    /* Used to compute block size.     */
	long s_log_frag_size;              /* Used to compute fragment size.  */
	unsigned long s_blocks_per_group;  /* Blocks per group.               */
	unsigned long s_frags_per_group;   /* Fragments per group.            */
	unsigned long s_inodes_per_group;  /* Inodes per group.               */
	unsigned long s_mtime;             /* Time of last mount.             */
	unsigned long s_wtime;             /* Time of last write.             */
	unsigned short s_mnt_count;        /* Mounts since last fsck.         */
	unsigned short s_max_mnt_count;    /* Mounts permitted between fscks. */
	unsigned short s_magic;            /* Identifies as ext2.             */
	unsigned short s_state;            /* Cleanly unmounted?              */
	unsigned short s_errors;           /* What to do on error.            */
	unsigned short s_minor_rev_level;  /* Minor revision level.           */
	unsigned long s_lastcheck;         /* Time of last fsck.              */
	unsigned long s_checkinterval;     /* Time permitted between fscks.   */
	unsigned long s_creator_os;        /* OS that created file system.    */
	unsigned long s_rev_level;         /* Revision level.                 */
	unsigned short s_def_resuid;       /* UID for reserved blocks.        */
	unsigned short s_def_resgid;       /* GID for reserved blocks.        */
	unsigned long s_first_ino;         /* First usable inode.             */
	unsigned short s_inode_size;       /* Size of inode struct.           */
	unsigned short s_block_group_nr;   /* Block group of this superblock. */
	unsigned long s_feature_compat;    /* Compatible features.            */
	unsigned long s_feature_incompat;  /* Incompatible features.          */
	unsigned long s_feature_ro_compat; /* Read-only features.             */
	char s_uuid[16];                   /* Volume ID.                      */
	char s_volume_name[16];            /* Volume name.                    */
	char s_last_mounted[64];           /* Path where last mounted.        */
	unsigned long s_algo_bitmap;       /* Compression methods.            */

	/* The following fields do not reside on the device: */
	dev_t dev;                         /* Device containing file system. */
	blksize_t block_size;              /* Block size.                    */
	unsigned long frag_size;           /* Fragment size.                 */
	inode_t *mount_point_inode_ptr;    /* Inode mounted on.              */
	inode_t *root_dir_inode_ptr;       /* Inode of root directory.       */
	bool dirty;                        /* Superblock changed since read. */
} super_t;

/* Value for s_magic: */
#define EXT2_SUPER_MAGIC	0xEF53 /* Identifies as ext2. */

/* Values for s_state: */
#define EXT2_VALID_FS		1 /* Cleanly unmounted.              */
#define EXT2_ERROR_FS		2 /* Mounted or uncleanly unmounted. */

/* Values for s_errors: */
#define EXT2_ERRORS_CONTINUE	1 /* Continue.                    */
#define EXT2_ERRORS_RO		2 /* Remount read-only.           */
#define EXT2_ERRORS_PANIC	3 /* Kernel panic.                */
#define EXT2_ERRORS_DEFAULT	1 /* Default behavior (continue). */

/* Values for s_creator_os: */
#define EXT2_OS_LINUX		0 /* Linux.       */
#define EXT2_OS_HURD		1 /* Hurd.        */
#define EXT2_OS_MASIX		2 /* MASIX.       */
#define EXT2_OS_FREEBSD		3 /* FreeBSD.     */
#define EXT2_OS_LITES4		4 /* Lites.       */
#define EXT2_OS_BRAINIX		5 /* Brainix! :-) */

/* Values for s_rev_level: */
#define EXT2_GOOD_OLD_REV	0 /* Original format.               */
#define EXT2_DYNAMIC_REV	1 /* V2 (with dynamic inode sizes). */

/* Superblock table: */
super_t super[NUM_SUPERS];

/* Function prototypes: */
void super_init(void);
super_t *super_get(dev_t dev);
super_t *super_read(dev_t dev);
void super_write(super_t *super_ptr);

#endif
