/*----------------------------------------------------------------------------*\
 |	inode.h								      |
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

#ifndef _FS_INODE_H
#define _FS_INODE_H

#include <sys/types.h>
#include <stdbool.h>
#include <config.h>

/* The first few inodes on a device are reserved: */
#define EXT2_BAD_INO		1 /* Bad blocks inode.         */
#define EXT2_ROOT_INO		2 /* Root directory inode.     */
#define EXT2_ACL_IDX_INO	3 /* ACL index inode.          */
#define EXT2_ACL_DATA_INO	4 /* ACL data inode.           */
#define EXT2_BOOT_LOADER_INO	5 /* Boot loader inode.        */
#define EXT2_UNDEL_DIR_INO	6 /* Undelete directory inode. */

/* An inode represents an object in the file system: */
typedef struct
{
	/* The following fields reside on the device: */
	unsigned short i_mode;        /* File format / access rights.       */
	unsigned short i_uid;         /* User owning file.                  */
	unsigned long i_size;         /* File size in bytes.                */
	unsigned long i_atime;        /* Access time.                       */
	unsigned long i_ctime;        /* Creation time.                     */
	unsigned long i_mtime;        /* Modification time.                 */
	unsigned long i_dtime;        /* Deletion time (0 if file exists).  */
	unsigned short i_gid;         /* Group owning file.                 */
	unsigned short i_links_count; /* Links count.                       */
	unsigned long i_blocks;       /* 512-byte blocks reserved for file. */
	unsigned long i_flags;        /* How to treat file.                 */
	unsigned long i_osd1;         /* OS dependant value.                */
	unsigned long i_block[15];    /* File data blocks.                  */
	unsigned long i_generation;   /* File version (used by NFS).        */
	unsigned long i_file_acl;     /* File ACL.                          */
	unsigned long i_dir_acl;      /* Directory ACL.                     */
	unsigned long i_faddr;        /* Fragment address.                  */
	unsigned long i_osd2[3];      /* OS dependant structure.            */

	/* The following fields do not reside on the device: */
	dev_t dev;                    /* Device the inode is on.            */
	ino_t ino;                    /* Inode number on its device.        */
	unsigned char count;          /* Number of times the inode is used. */
	bool mounted;                 /* Inode is mounted on.               */
	bool dirty;                   /* Inode changed since read.          */
} inode_t;

/* Values for i_mode (file format): */
#define EXT2_S_IFMT		0xF000 /* Format mask.      */
#define EXT2_S_IFSOCK		0xC000 /* Socket.           */
#define EXT2_S_IFLNK		0xA000 /* Symbolic link.    */
#define EXT2_S_IFREG		0x8000 /* Regular file.     */
#define EXT2_S_IFBLK		0x6000 /* Block device.     */
#define EXT2_S_IFDIR		0x4000 /* Directory.        */
#define EXT2_S_IFCHR		0x2000 /* Character device. */
#define EXT2_S_IFIFO		0x1000 /* FIFO.             */

/* Values for i_mode (access rights): */
#define EXT2_S_ISUID		0x0800 /* SUID.                      */
#define EXT2_S_ISGID		0x0400 /* SGID.                      */
#define EXT2_S_ISVTX		0x0200 /* Sticky bit.                */
#define EXT2_S_IRWXU		0x01C0 /* User access rights mask.   */
#define EXT2_S_IRUSR		0x0100 /* User read.                 */
#define EXT2_S_IWUSR		0x0080 /* User write.                */
#define EXT2_S_IXUSR		0x0040 /* User execute.              */
#define EXT2_S_IRWXG		0x0038 /* Group access rights mask.  */
#define EXT2_S_IRGRP		0x0020 /* Group read.                */
#define EXT2_S_IWGRP		0x0010 /* Group write.               */
#define EXT2_S_IXGRP		0x0008 /* Group execute.             */
#define EXT2_S_IRWXO		0x0007 /* Others access rights mask. */
#define EXT2_S_IROTH		0x0004 /* Others read.               */
#define EXT2_S_IWOTH		0x0002 /* Others write.              */
#define EXT2_S_IXOTH		0x0001 /* Others execute.            */

/* Values for i_flags: */
#define EXT2_SECRM_FL		0x00000001 /* Delete securely.            */
#define EXT2_UNRM_FL		0x00000002 /* Record for undelete.        */
#define EXT2_COMPR_FL		0x00000004 /* ?                           */
#define EXT2_SYNC_FL		0x00000008 /* Update synchronously.       */
#define EXT2_IMMUTABLE_FL	0x00000010 /* ?                           */
#define EXT2_APPEND_FL		0x00000020 /* Only append to file.        */
#define EXT2_NODUMP_FL		0x00000040 /* Do not dump/delete file.    */
#define EXT2_NOATIME_FL		0x00000080 /* Do not update access time.  */
#define EXT2_DIRTY_FL		0x00000100 /* ?                           */
#define EXT2_COMPRBLK_FL	0x00000200 /* ?                           */
#define EXT2_NOCOMPR_FL		0x00000400 /* Access raw compressed data. */
#define EXT2_ECOMPR_FL		0x00000800 /* ?                           */
#define EXT2_BTREE_FL		0x00010000 /* ?                           */
#define EXT2_INDEX_FL		0x00010000 /* ?                           */
#define EXT2_IMAGIC_FL		0x00020000 /* ?                           */
#define EXT2_JOURNAL_DATA_FL	0x00040000 /* ?                           */
#define EXT2_RESERVED_FL	0x80000000 /* Reserved.                   */

#define NUM_DIRECT(s)		12
#define NUM_INDIRECT(s)		(s / sizeof(blkcnt_t))
#define NUM_BI_INDIRECT(s)	(NUM_INDIRECT(s) * NUM_INDIRECT(s))
#define NUM_TRI_INDIRECT(s)	(NUM_BI_INDIRECT(s) * NUM_INDIRECT(s))

#define SUB_DIRECT(s)		0
#define SUB_INDIRECT(s)		(NUM_DIRECT(s))
#define SUB_BI_INDIRECT(s)	(SUB_INDIRECT(s) + NUM_INDIRECT(s))
#define SUB_TRI_INDIRECT(s)	(SUB_BI_INDIRECT(s) + NUM_BI_INDIRECT(s))

#define IS_DIRECT(s, n)		(n - SUB_DIRECT(s) < NUM_DIRECT(s))
#define IS_INDIRECT(s, n)	(n - SUB_INDIRECT(s) < NUM_INDIRECT(s))
#define IS_BI_INDIRECT(s, n)	(n - SUB_BI_INDIRECT(s) < NUM_BI_INDIRECT(s))
#define IS_TRI_INDIRECT(s, n)	(n - SUB_TRI_INDIRECT(s) < NUM_TRI_INDIRECT(s))

/* Inode table: */
inode_t inode[NUM_INODES];

/* Function prototypes: */
void inode_init(void);
void inode_rw(inode_t *inode_ptr, bool read);
inode_t *inode_get(dev_t dev, ino_t ino);
void inode_put(inode_t *inode_ptr);
blkcnt_t block_find(inode_t *inode_ptr, blkcnt_t n);
bool is_blk(inode_t *inode_ptr);
bool is_dir(inode_t *inode_ptr);
bool is_chr(inode_t *inode_ptr);

#endif
