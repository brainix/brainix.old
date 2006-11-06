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

#include <driver/driver.h>
#include <driver/keymap.h>

/* Keyboard state: */
unsigned char ctrl   = 0; /* Non-zero while Ctrl pressed.   */
unsigned char shift  = 0; /* Non-zero while Shift pressed.  */
unsigned char alt    = 0; /* Non-zero while Alt pressed.    */
unsigned char caps   = 0; /* Non-zero while Caps Lock on.   */
unsigned char num    = 0; /* Non-zero while Num Lock on.    */
unsigned char scroll = 0; /* Non-zero while Scroll Lock on. */

/* Global variables: */
unsigned char prev_scode = 0xE0; /* Scancode previously generated.    */
unsigned char prev_ascii = 0xE0; /* ASCII value previously generated. */
unsigned char curr_scode = 0xE0; /* Scancode being generated.         */
unsigned char curr_ascii = 0xE0; /* ASCII value being generated.      */

/* Function prototypes: */
unsigned char scode_to_ascii(unsigned char scode);
bool kbd_state(void);
void kbd_irq(unsigned char scode);

/*----------------------------------------------------------------------------*\
 |				scode_to_ascii()			      |
\*----------------------------------------------------------------------------*/
unsigned char scode_to_ascii(unsigned char scode)
{

/* Return a scancode's ASCII value, accounting for the keyboard state.  Return 0
 * if the scancode has no associated ASCII value. */

	unsigned char ascii;

	if (scode > 0x5E)
		/* Break code or invalid scancode. */
		return 0;

	/* Assume no keyboard state modifiers. */
	ascii = key_map[scode];

	/* Now account for the keyboard state. */
	if (ctrl)
		return ctrl_map[scode];
	if (shift && shift_map[scode])
		ascii = shift_map[scode];
	if (alt)
		return alt_map[scode];
	if (caps && caps_map[scode])
		ascii = caps_map[scode];
	if (num && num_map[scode])
		ascii = num_map[scode];

	/* Return the scancode's ASCII value. */
	return ascii;
}

/*----------------------------------------------------------------------------*\
 |				  kbd_state()				      |
\*----------------------------------------------------------------------------*/
bool kbd_state(void)
{

/* Check the key being pressed/released and, if necessary, modify the keyboard
 * state. */

	bool release = curr_scode & BREAK;

	if (prev_scode == curr_scode)
		return 0;

	switch (curr_scode & ~BREAK)
	{
		case CTRL    : ctrl  += release ? -1     : 1;       return 1;
		case L_SHIFT : shift += release ? -1     : 1;       return 1;
		case R_SHIFT : shift += release ? -1     : 1;       return 1;
		case ALT     : alt   += release ? -1     : 1;       return 1;
		case CAPS    : caps   = release ? caps   : !caps;   break;
		case NUM     : num    = release ? num    : !num;    break;
		case SCROLL  : scroll = release ? scroll : !scroll; break;
		default      :                                      return 0;
	}

	if (!release)
		kbd_leds(caps, num, scroll);
	return 1;
}

/*----------------------------------------------------------------------------*\
 |				   kbd_irq()				      |
\*----------------------------------------------------------------------------*/
void kbd_irq(unsigned char scode)
{

/* The keyboard has generated an interrupt (a key is being pressed, is
 * repeating, or is being released).  Handle the interrupt. */

	msg_t *msg;

	/* Save the previous scancode and ASCII value, and get the current
	 * scancode and ASCII value. */
	prev_scode = curr_scode;
	prev_ascii = curr_ascii;
	curr_scode = scode;
	curr_ascii = scode_to_ascii(scode);

	/* If a Windows key is being pressed, spontaneously reboot. */
	if (curr_scode == L_WIN || curr_scode == R_WIN || curr_scode == MENU)
	{
		(msg = msg_alloc(KERNEL_PID, SHUTDOWN))->from = TTY_PID;
		msg->args.brnx_shutdown.reboot = true;
		msg_send(msg);
		return;
	}

	if (kbd_state())
		return;

	if (ctrl)
		switch (curr_ascii)
		{
			case 'g': beep(440, 5);    return;
			case 'h': printf("\b \b"); return;
		}

/*
	if (curr_ascii)
		putchar(curr_ascii);
 */
}
