/*----------------------------------------------------------------------------*\
 |	driver.c - device independent driver code			      |
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

#include <driver/driver.h>
#include <lib/string.h>

/* Function prototypes: */
void drvr_reg_kernel(unsigned char irq);
void drvr_reg_fs(bool block, unsigned char maj);
void drvr_set_alarm(clock_t ticks, unsigned char type);
void drvr_set_wait_alarm(clock_t ticks, unsigned char type,
	unsigned char (*handle_alarm)(msg_t *msg));
void drvr_do_nothing(void);
void drvr_main(drvr_t *drvr);

/*----------------------------------------------------------------------------*\
 |			       drvr_reg_kernel()			      |
\*----------------------------------------------------------------------------*/
void drvr_reg_kernel(unsigned char irq)
{

/* Register a driver with the kernel.  This is only required of drivers for
 * devices which use IRQs. */

	/* Send a register message to the kernel. */
	msg_t *msg = msg_alloc(KERNEL_PID, REGISTER);
	msg->args.brnx_reg.irq = irq;
	msg_send(msg);

	/* Await the kernel's reply. */
	msg = msg_receive(KERNEL_PID);
	msg_free(msg);

	/* Now the kernel will send messages to the current (device driver)
	 * process to report the specified IRQ. */
}

/*----------------------------------------------------------------------------*\
 |				 drvr_reg_fs()				      |
\*----------------------------------------------------------------------------*/
void drvr_reg_fs(bool block, unsigned char maj)
{

/* Register a driver with the file system. */

	/* Send a register message to the file system. */
	msg_t *msg = msg_alloc(FS_PID, REGISTER);
	msg->args.brnx_reg.block = block;
	msg->args.brnx_reg.maj = maj;
	msg_send(msg);
}

/*----------------------------------------------------------------------------*\
 |				drvr_set_alarm()			      |
\*----------------------------------------------------------------------------*/
void drvr_set_alarm(clock_t ticks, unsigned char type)
{

/* Set an alarm. */

	/* Send an alarm message to the timer. */
	msg_t *msg = msg_alloc(TMR_PID, WATCHDOG);
	msg->args.brnx_watch.ticks = ticks;
	msg->args.brnx_watch.type = type;
	msg_send(msg);
}

/*----------------------------------------------------------------------------*\
 |			     drvr_set_wait_alarm()			      |
\*----------------------------------------------------------------------------*/
void drvr_set_wait_alarm(clock_t ticks, unsigned char type,
	unsigned char (*handle_alarm)(msg_t *msg))
{

/* Set an alarm, then wait for it to sound. */

	msg_t *msg;
	bool done;

	/* Set the alarm. */
	drvr_set_alarm(ticks, type);

	/* Wait for it to sound. */
	do
	{
		msg = msg_receive(TMR_PID);
		done = (*handle_alarm)(msg) == type;
		msg_free(msg);
	} while (!done);
}

/*----------------------------------------------------------------------------*\
 |			       drvr_do_nothing()			      |
\*----------------------------------------------------------------------------*/
void drvr_do_nothing(void)
{

/* Do nothing.  Wow, that was easy. */

}

/*----------------------------------------------------------------------------*\
 |				  drvr_main()				      |
\*----------------------------------------------------------------------------*/
void drvr_main(drvr_t *drvr)
{
	/* Initialize the driver. */
	(*drvr->init)();

	/* Wait for a message. */
	while ((*drvr->msg = msg_receive(ANYONE))->op != SHUTDOWN)
	{
		/* Do the requested work.  Send a reply (if necessary). */
		(*drvr->prepare)();
		switch ((*drvr->msg)->op)
		{
			case IRQ:
				(*drvr->irq)();
				msg_free(*drvr->msg);
				break;
			case WATCHDOG:
				(*drvr->alarm)();
				msg_free(*drvr->msg);
				break;
			case SYS_OPEN:
				(*drvr->open)();
				msg_reply(*drvr->msg);
				break;
			case SYS_CLOSE:
				(*drvr->close)();
				msg_reply(*drvr->msg);
				break;
			case SYS_READ:
				(*drvr->read)();
				msg_reply(*drvr->msg);
				break;
			case SYS_WRITE:
				(*drvr->write)();
				msg_reply(*drvr->msg);
				break;
			case SYS_IOCTL:
				(*drvr->ioctl)();
				msg_reply(*drvr->msg);
				break;
			default:
				//char* foo = strcat("unexpected message: ", ((*drvr->msg)->op));
				scream("drvr_main", "unexpected message", "driver");
		}
		(*drvr->cleanup)();
	}

	/* Deinitialize the driver. */
	(*drvr->deinit)();
	msg_reply(*drvr->msg);
	do_exit(0);
}
