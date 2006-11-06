/*----------------------------------------------------------------------------*\
 |	memory.c							      |
 |									      |
 |	Copyright � 2002-2006, Team Brainix, original authors.		      |
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

#include <driver/driver.h>

/* Major number: */
#define MAJ		  1

/* Minor numbers: */
#define MIN_RAM		  0
#define MIN_MEM		  1
#define MIN_KMEM	  2
#define MIN_NULL	  3
#define MIN_PORT	  4
#define MIN_ZERO	  5
#define MEM_FULL	  7
#define MEM_RANDOM	  8
#define MEM_URANDOM	  9
#define MEM_INITRD	250

/* Global variable: */
msg_t *msg;

/* Function prototype: */
void mem_init(void);
void mem_main(void);

/*----------------------------------------------------------------------------*\
 |				   mem_init()				      |
\*----------------------------------------------------------------------------*/
void mem_init(void)
{
	/* Register the memory driver with the file system. */
	drvr_reg_fs(CHAR, MAJ);
}

/*----------------------------------------------------------------------------*\
 |				   mem_main()				      |
\*----------------------------------------------------------------------------*/
void mem_main(void)
{
	drvr_t drvr =
	{
		&msg,
		mem_init,        /* init()    */
		drvr_do_nothing, /* prepare() */
		drvr_do_nothing, /* irq()     */
		drvr_do_nothing, /* alarm()   */
		drvr_do_nothing, /* open()    */
		drvr_do_nothing, /* close()   */
		drvr_do_nothing, /* read()    */
		drvr_do_nothing, /* write()   */
		drvr_do_nothing, /* ioctl()   */
		drvr_do_nothing, /* cleanup() */
		drvr_do_nothing  /* deinit()  */
	};

	drvr_main(&drvr);
}
