/*----------------------------------------------------------------------------*\
 |	keyboard.c							      |
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

#define BRAINIX
#include <kernel/kernel.h>

/* Registers: */
#define KBD_DATA_PORT	0x60 /* Data register.    (R  ) */
#define KBD_CMD_PORT	0x60 /* Command register. (  W) */
#define KBD_STAT_PORT	0x64 /* Status register.  (R  ) */
#define KBD_CTRL_PORT	0x64 /* Control register. (  W) */

/* Values to read from / write to registers: */
#define KBD_CMD_LED	0xED /* Set LEDs.                 */
#define KBD_CMD_TYPE	0xF3 /* Set typematic delay/rate. */
#define KBD_CTRL_REBOOT	0xFE /* Reboot system.            */

/* True once the keyboard is ready: */
#define KBD_READY(kbd_stat)	((kbd_stat & 0x02) == 0x00)

/* True once the keyboard has acknowledged: */
#define KBD_ACK(kbd_data)	(kbd_data == 0xFA)

/* Function prototypes: */
void kbd_out(unsigned char value, unsigned short port);
void kbd_reboot(void);
void kbd_leds(bool caps, bool num, bool scroll);
void kbd_handler(void);
void kbd_init(void);

/*----------------------------------------------------------------------------*\
 |				   kbd_out()				      |
\*----------------------------------------------------------------------------*/
void kbd_out(unsigned char value, unsigned short port)
{

/* Wait until the keyboard is ready, send a command or control byte, then wait
 * for the keyboard to acknowledge. */

	/* Wait until the keyboard is ready. */
	while (!KBD_READY(in_byte(KBD_STAT_PORT)))
		;

	/* Send the command or control byte. */
	out_byte(value, port);

	/* Wait for the keyboard to acknowledge. */
	if (value != KBD_CTRL_REBOOT)
		while (!KBD_ACK(in_byte(KBD_DATA_PORT)))
			;
}

/*----------------------------------------------------------------------------*\
 |				  kbd_reboot()				      |
\*----------------------------------------------------------------------------*/
void kbd_reboot(void)
{

/* Reboot the system. */

	kbd_out(KBD_CTRL_REBOOT, KBD_CTRL_PORT);
}

/*----------------------------------------------------------------------------*\
 |				   kbd_leds()				      |
\*----------------------------------------------------------------------------*/
void kbd_leds(bool caps, bool num, bool scroll)
{

/* Set the LEDs to reflect the keyboard state. */

	/* Encode the LED bits. */
	unsigned char led_code = (caps << 2) | (num << 1) | scroll;

	/* Set the LEDs to the encoded value. */
	kbd_out(KBD_CMD_LED, KBD_CMD_PORT);
	kbd_out(led_code, KBD_CMD_PORT);
}

/*----------------------------------------------------------------------------*\
 |				 kbd_handler()				      |
\*----------------------------------------------------------------------------*/
void kbd_handler(void)
{

/* The keyboard has generated an interrupt (a key is being pressed or released).
 * Handle the interrupt. */

	unsigned char scode = in_byte(KBD_DATA_PORT);
	msg_t *msg;

	if (scode == 0xE0)
		/* The keyboard is telling us that the upcoming key did not
		 * exist on the original AT keyboard.  Raise your hand if you
		 * give a fuck.  Thank you. */
		;
	else
	{
		/* Send the scancode to the terminal driver for further
		 * processing. */
		(msg = msg_alloc(TTY_PID, IRQ))->from = HARDWARE;
		msg->args.brnx_irq.scode = scode;
		msg_send(msg);
	}
	irq_eoi(KBD_IRQ);
}

/*----------------------------------------------------------------------------*\
 |				   kbd_init()				      |
\*----------------------------------------------------------------------------*/
void kbd_init(void)
{

/* Initialize the keyboard. */

	/* Turn off the Num Lock LED (if the BIOS left it on). */
	kbd_leds(false, false, false);

	/* Set the typematic delay to 0.25 seconds and the typematic rate to 30
	 * keys per second. */
	kbd_out(KBD_CMD_TYPE, KBD_CMD_PORT);
	kbd_out(0x00, KBD_CMD_PORT);

	/* Set the interrupt handler and enable the IRQ. */
	intr_set(KBD_INTR, (unsigned long) kbd_wrapper, KERNEL_PL);
	irq_enable(KBD_IRQ);
}
