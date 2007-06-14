/*----------------------------------------------------------------------------*\
 |	fs.h								      |
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

#ifndef _FS_H
#define _FS_H

#include <kernel/kernel.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/stat.h>
#include <unistd.h>

#include <fs/block.h>
#include <fs/device.h>
#include <fs/dir.h>
#include <fs/elf.h>
#include <fs/fildes.h>
#include <fs/group.h>
#include <fs/inode.h>
#include <fs/link.h>
#include <fs/main.h>
#include <fs/misc.h>
#include <fs/mount.h>
#include <fs/open.h>
#include <fs/path.h>
#include <fs/perm.h>
#include <fs/proc.h>
#include <fs/read.h>
#include <fs/search.h>
#include <fs/stat.h>
#include <fs/super.h>
#include <fs/time.h>

#endif
