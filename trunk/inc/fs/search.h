/*----------------------------------------------------------------------------*\
 |	search.h							      |
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

#ifndef _FS_SEARCH_H
#define _FS_SEARCH_H

#include <sys/types.h>
#include <fs/inode.h>

/* A directory's data blocks are filled with directory entries, each
 * representing a file in the directory: */
typedef struct
{
	unsigned long inode;     /* Inode number.         */
	unsigned short rec_len;  /* Offset to next entry. */
	unsigned char name_len;  /* File name length.     */
	unsigned char file_type; /* File type.            */
	char *name;              /* File name.            */
} dir_entry_t;

/* Values for file_type: */
#define EXT2_FT_UNKNOWN		0 /* Unknown.          */
#define EXT2_FT_REG_FILE	1 /* Regular file.     */
#define EXT2_FT_DIR		2 /* Directory.        */
#define EXT2_FT_CHRDEV		3 /* Character device. */
#define EXT2_FT_BLKDEV		4 /* Block device.     */
#define EXT2_FT_FIFO		5 /* FIFO.             */
#define EXT2_FT_SOCK		6 /* Socket.           */
#define EXT2_FT_SYMLINK		7 /* Symbolic link.    */
#define EXT2_FT_MAX		8 /* ?                 */

/* Function prototypes: */
ino_t search_block(dev_t dev, blkcnt_t blk, char *name);
ino_t search_dir(inode_t *inode_ptr, char *name);

#endif
