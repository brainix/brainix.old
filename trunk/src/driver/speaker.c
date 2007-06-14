/*----------------------------------------------------------------------------*\
 |	speaker.c							      |
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

#define PORT_B			0x61
#define ENABLE_SPEAKER(port_b)	(port_b | 0x03)
#define DISABLE_SPEAKER(port_b)	(port_b & ~0x03)

/* Alarm types: */
#define ALARM_FREQ_SET		0 /* The frequency has been set. */
#define ALARM_SPEAKER_OFF	1 /* Turn the speaker off.       */

/* Global variables: */
unsigned short old_freq = 0;
bool beeping = false;

/* Function prototypes: */
void speaker_freq(unsigned short new_freq);
void speaker_on(void);
void speaker_off(void);
void beep(unsigned short freq, clock_t ticks);

/*----------------------------------------------------------------------------*\
 |				 speaker_freq()				      |
\*----------------------------------------------------------------------------*/
void speaker_freq(unsigned short new_freq)
{

/* Set the frequency for the PC speaker. */

	msg_t *msg;
	mid_t mid;

	if (old_freq == new_freq)
		return;

	/* Politely ask the timer to set the frequency. */
	msg = msg_alloc(TMR_PID, FREQUENCY);
	msg->args.brnx_freq.counter = 2;
	msg->args.brnx_freq.freq = old_freq = new_freq;
	msg_send(msg);

	/*
	 | The rest of this function is a hack.  We can't return until the
	 | frequency has been set, but the timer doesn't reply to frequency set
	 | requests.  So we send a second request for an alarm to sound now.
	 | When the alarm sounds, the frequency will have been set, and we'll be
	 | able to return.
	 */

	/* Politely ask the timer to sound an alarm. */
	mid = (msg = msg_alloc(TMR_PID, WATCHDOG))->mid;
	msg->args.brnx_watch.ticks = 0;
	msg->args.brnx_watch.type = ALARM_FREQ_SET;
	msg_send(msg);

	/* Anxiously await the alarm. */
	msg = msg_receive(mid);
	msg_free(msg);
}

/*----------------------------------------------------------------------------*\
 |				  speaker_on()				      |
\*----------------------------------------------------------------------------*/
void speaker_on(void)
{

/* Turn on the PC speaker. */

	out_byte(ENABLE_SPEAKER(in_byte(PORT_B)), PORT_B);
	beeping = true;
}

/*----------------------------------------------------------------------------*\
 |				 speaker_off()				      |
\*----------------------------------------------------------------------------*/
void speaker_off(void)
{

/* Turn off the PC speaker. */

	out_byte(DISABLE_SPEAKER(in_byte(PORT_B)), PORT_B);
	beeping = false;
}

/*----------------------------------------------------------------------------*\
 |				     beep()				      |
\*----------------------------------------------------------------------------*/
void beep(unsigned short freq, clock_t ticks)
{

/* Beep at the requested frequency, for the requested time interval. */

	msg_t *msg;

	if (beeping)
		/* Prevent that "BEE-BRR-EEP!" pile-up. */
		return;

	/* Set the frequency. */
	speaker_freq(freq);

	/* Turn on the speaker. */
	speaker_on();

	/* Schedule the speaker to be turned off. */
	msg = msg_alloc(TMR_PID, WATCHDOG);
	msg->args.brnx_watch.ticks = ticks;
	msg->args.brnx_watch.type = ALARM_SPEAKER_OFF;
	msg_send(msg);
}
