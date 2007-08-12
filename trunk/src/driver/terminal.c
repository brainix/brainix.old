/*----------------------------------------------------------------------------*\
 |	terminal.c							      |
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

#include <driver/driver.h>

/* Major number: */
#define TTY_MAJ		5

/* Global variable: */
msg_t *msg;

/* Function prototypes: */
void tty_init(void);
void tty_irq(void);
void tty_open(void);
void tty_close(void);
void tty_read(void);
void tty_write(void);
void tty_ioctl(void);
void tty_main(void);

/*----------------------------------------------------------------------------*\
 |				   tty_init()				      |
\*----------------------------------------------------------------------------*/
void tty_init(void)
{
	/* Register this driver with the file system. */
	drvr_reg_fs(CHAR, TTY_MAJ);

	/* Initialize the keyboard. */
	kbd_init();
}

/*----------------------------------------------------------------------------*\
 |				   tty_irq()				      |
\*----------------------------------------------------------------------------*/
void tty_irq(void)
{
	kbd_irq(msg->args.brnx_irq.scode);
}

/*----------------------------------------------------------------------------*\
 |				   tty_open()				      |
\*----------------------------------------------------------------------------*/
void tty_open(void)
{
}

/*----------------------------------------------------------------------------*\
 |				  tty_close()				      |
\*----------------------------------------------------------------------------*/
void tty_close(void)
{
}

/*----------------------------------------------------------------------------*\
 |				   tty_read()				      |
\*----------------------------------------------------------------------------*/
void tty_read(void)
{
}

/*----------------------------------------------------------------------------*\
 |				  tty_write()				      |
\*----------------------------------------------------------------------------*/
void tty_write(void)
{
}

/*----------------------------------------------------------------------------*\
 |				  tty_ioctl()				      |
\*----------------------------------------------------------------------------*/
void tty_ioctl(void)
{
	switch (msg->args.ioctl.request)
	{
		default:
			msg->args.ioctl.ret_val = -ENOTTY;
	}
}

/*----------------------------------------------------------------------------*\
 |				   tty_main()				      |
\*----------------------------------------------------------------------------*/
void tty_main(void)
{
	drvr_t drvr =
	{
		&msg,
		tty_init,        /* init()    */
		drvr_do_nothing, /* prepare() */
		tty_irq,         /* irq()     */
		speaker_off,     /* alarm()   */
		tty_open,        /* open()    */
		tty_close,       /* close()   */
		tty_read,        /* read()    */
		tty_write,       /* write()   */
		tty_ioctl,       /* ioctl()   */
		drvr_do_nothing, /* cleanup() */
		drvr_do_nothing  /* deinit()  */
	};

	drvr_main(&drvr);
}
