/*----------------------------------------------------------------------------*\
 |	const.h								      |
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

#include <stdbool.h>

/* Memory units: */
#define B		((unsigned long) 1) /* Byte.     */
#define KB		(1024 * B)          /* Kilobyte. */
#define MB		(1024 * KB)         /* Megabyte. */
#define GB		(1024 * MB)         /* Gigabyte. */

/* Time conversion: */
#define SECS_PER_MIN	60                   /* Seconds per minute. */
#define SECS_PER_HOUR	(SECS_PER_MIN * 60)  /* Seconds per hour.   */
#define SECS_PER_DAY	(SECS_PER_HOUR * 24) /* Seconds per day.    */

/* Time info: */
#define SEC		0 /* Second.        */
#define SEC_ALARM	1 /* Alarm: second. */
#define MIN		2 /* Minute.        */
#define MIN_ALARM	3 /* Alarm: minute. */
#define HOUR		4 /* Hour.          */
#define HOUR_ALARM	5 /* Alarm: hour.   */
#define WDAY		6 /* Day of week.   */
#define MDAY		7 /* Day of month.  */
#define MON		8 /* Month.         */
#define YEAR		9 /* Year.          */

/* Segments: */
#define UNUSED_SEG	0 /* Unused segment.      */
#define KERNEL_CODE_SEG	1 /* Kernel code segment. */
#define KERNEL_DATA_SEG	2 /* Kernel data segment. */
#define USER_CODE_SEG	3 /* User code segment.   */
#define USER_DATA_SEG	4 /* User data segment.   */
#define NUM_SEGS	5 /* Number of segments.  */

/* True iff the specified segment is a kernel segment: */
#define KERNEL_SEG(j)	(j == KERNEL_CODE_SEG || j == KERNEL_DATA_SEG)

/* True iff the specified segment is a code segment: */
#define CODE_SEG(j)	(j == KERNEL_CODE_SEG || j == USER_CODE_SEG)

/* IRQ numbers: */
#define TMR_IRQ		 0 /* Timer.    */
#define KBD_IRQ		 1 /* Keyboard. */
#define SLV_IRQ		 2 /* Slave.    */
#define DSP_IRQ		 5 /* Sound.    */
#define FDC_IRQ		 6 /* Floppy.   */

/* Interrupt vectors: */
#define TMR_INTR	 32 /* Timer.                       */
#define KBD_INTR	 33 /* Keyboard.                    */
#define SYS_INTR	128 /* System call.                 */
#define NUM_INTRS	256 /* Number of interrupt vectors. */

/* DMA channels: */
#define DSP_DMA		1 /* Sound.  */
#define FDC_DMA		2 /* Floppy. */

/* Task states: */
#define NONEXIST	0 /* */
#define BLOCKED		1 /* */
#define READY		2 /* */
#define ZOMBIE		3 /* */

/* Task info: */
#define CURRENT_TASK	-1 /* Current task.                              */
#define EGID		 0 /* Effective group ID.                        */
#define EUID		 1 /* Effective user ID.                         */
#define GID		 2 /* Real group ID.                             */
#define PGRP		 3 /* Process group ID.                          */
#define PID		 4 /* Process ID.                                */
#define PPID		 5 /* Parent process ID.                         */
#define UID		 6 /* Real user ID.                              */
#define UTIME		 7 /* User time.                                 */
#define STIME		 8 /* System time.                               */
#define CUTIME		 9 /* User time of terminated child processes.   */
#define CSTIME		10 /* System time of terminated child processes. */
#define STATE		11 /* Task state.                                */

/* Background/foreground colors: */
#define BLACK		0x0
#define BLUE		0x1
#define GREEN		0x2
#define CYAN		0x3
#define RED		0x4
#define MAGENTA		0x5
#define BROWN		0x6
#define WHITE		0x7
#define DARK_GRAY	0x8
#define BRIGHT_BLUE	0x9
#define BRIGHT_GREEN	0xA
#define BRIGHT_CYAN	0xB
#define PINK		0xC
#define BRIGHT_MAGENTA	0xD
#define YELLOW		0xE
#define BRIGHT_WHITE	0xF

/* Emoticons: */
#define ERASE		0 /*     */
#define SMILE		1 /* :-) */
#define THINK		2 /* :-\ */
#define FROWN		3 /* :-( */
#define SHOUT		4 /* >:o */

/* Process IDs: */
#define ANYONE		-2
#define HARDWARE	-1
#define BOOT_PID	 0
#define IDLE_PID	 1
#define KERNEL_PID	 2
#define TMR_PID	 	 3
#define FS_PID		 4
#define MEM_PID		 5
#define FDC_PID		 6
#define TTY_PID		 7
#define INIT_PID	 8

#define KERNEL		true
#define USER		false
#define HEAP		true
#define STACK		false
#define BOTTOM		true
#define TOP		false
#define BLOCK		true
#define CHAR		false
#define OPEN		true
#define CLOSE		false
#define READ		true
#define WRITE		false
#define BLINKING	true
#define SOLID		false

/* File system robustness values: */
#define SLOPPY		0
#define SANE		1
#define PARANOID	2
