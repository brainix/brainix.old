/*----------------------------------------------------------------------------*\
 |	kernel.h							      |
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

#ifndef _KERNEL_KERNEL_H
#define _KERNEL_KERNEL_H

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/types.h>
#include <utime.h>

#include <config.h>
#include <const.h>

#include <kernel/multiboot.h>
#include <kernel/struct.h>
#include <kernel/message.h>
#include <kernel/prototype.h>
#include <kernel/syscall.h>

#include <fs/fs.h>
#include <driver/driver.h>

#endif
