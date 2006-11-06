/*----------------------------------------------------------------------------*\
 |	timer.c - 82C54 PIT (Programmable Interval Timer) code		      |
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

#define TMR_BASE_FREQ		0x1234DD
#define TMR_PORT(counter)	(0x40 + counter)
#define TMR_CTRL_PORT		0x43
#define TMR_CTRL_WORD(sc, rw, m, bcd) \
	((sc << 6) | (rw << 4) | (m << 1) | bcd)

typedef struct alarm
{
	clock_t uptime;
	msg_t *msg;
	struct alarm *prev;
	struct alarm *next;
} alarm_t;

typedef struct sigalrm
{
	clock_t uptime;
	pid_t pid;
	struct sigalrm *prev;
	struct sigalrm *next;
} sigalrm_t;

/* Global variables: */
clock_t uptime = 0;
time_t secs_pending = 0;
alarm_t *next_alarm = NULL;
sigalrm_t *next_sigalrm = NULL;

/* Function prototypes: */
void timer_freq(unsigned char counter, unsigned short freq);
void timer_handler(void);
void timer_init(void);
void timer_deinit(msg_t *msg);
void timer_watch(msg_t *msg);
void timer_sigalrm(msg_t *msg);
void timer_main(void);
void timer_sleep(clock_t ticks);

/* Global variables and function prototypes in clock.c: */
extern time_t secs_since_epoch;
void cmos_get_time(void);
void cmos_set_time(void);
void cmos_update_time(time_t pending);

/*----------------------------------------------------------------------------*\
 |				  timer_freq()				      |
\*----------------------------------------------------------------------------*/
void timer_freq(unsigned char counter, unsigned short freq)
{
	unsigned char timer_ctrl_word = TMR_CTRL_WORD(counter, 3, 3, 0);
	unsigned short count = TMR_BASE_FREQ / freq;

	out_byte(timer_ctrl_word, TMR_CTRL_PORT);
	out_byte((count & 0x00FF) >> 0, TMR_PORT(counter));
	out_byte((count & 0xFF00) >> 8, TMR_PORT(counter));
}

/*----------------------------------------------------------------------------*\
 |				timer_handler()				      |
\*----------------------------------------------------------------------------*/
void timer_handler(void)
{

/*
 | The timer has ticked (generated an interrupt).  Update times, sound all due
 | alarms, generate all due SIGALRMs, and switch to the next process.  Then,
 | when the scheduler schedules us (the current process) again, deliver our
 | pending signals.
 |
 | Note: This function is called 60 times per second.  Keep it as efficient as
 |       possible.
 */

	alarm_t *alarm;
	sigalrm_t *sigalrm;

	intr_lock();

	/* Update times. */
	if (++uptime % HZ == 0)
		secs_pending++;
	proc_update();

	/* Sound all due alarms. */
	while (next_alarm != NULL && next_alarm->uptime <= uptime)
	{
		msg_reply(next_alarm->msg);
		if ((alarm = next_alarm->next) != NULL)
			alarm->prev = NULL;
		kfree(next_alarm);
		next_alarm = alarm;
	}

	/* Generate all due SIGALRMs. */
	while (next_sigalrm != NULL && next_sigalrm->uptime <= uptime)
	{
		sig_send(next_sigalrm->pid, SIGALRM);
		if ((sigalrm = next_sigalrm->next) != NULL)
			sigalrm->prev = NULL;
		kfree(next_sigalrm);
		next_sigalrm = sigalrm;
	}

	/* Switch to the next process. */
	irq_eoi(TMR_IRQ);
	intr_unlock();
	proc_sched();

	/* Deliver pending signals. */
	sig_deliver();
}

/*----------------------------------------------------------------------------*\
 |				  timer_init()				      |
\*----------------------------------------------------------------------------*/
void timer_init(void)
{
	cmos_get_time();
	timer_freq(0, HZ);
	intr_set(TMR_INTR, (unsigned long) timer_wrapper, KERNEL_PL);
	irq_enable(TMR_IRQ);

	/*
	 | [a steady, ominous downpour of rain]
	 |
	 | The Mad Scientist:
	 |     Now, Brainix has a heartbeat.  It's alive!  Mwa-ha-ha-ha-ha-ha!
	 |
	 | [a lightening bolt, followed by a thunder clap]
	 */
}

/*----------------------------------------------------------------------------*\
 |				 timer_deinit()				      |
\*----------------------------------------------------------------------------*/
void timer_deinit(msg_t *msg)
{
	intr_lock();
	cmos_set_time();
	intr_unlock();
	msg_reply(msg);
	while (true)
		msg_free(msg_receive(ANYONE));
}

/*----------------------------------------------------------------------------*\
 |				 timer_watch()				      |
\*----------------------------------------------------------------------------*/
void timer_watch(msg_t *msg)
{

/* Set an alarm -- set the specified message to be returned to its sending
 * process after the specified number of ticks. */

	clock_t uptime_to_send = uptime + msg->args.brnx_watch.ticks;
	alarm_t *tmp;
	alarm_t *new_alarm = NULL;

	intr_lock();

	/* If the requested alarm already exists, remove it from the list.
	 * We'll reinsert it later (to keep the list sorted by time). */
	for (tmp = next_alarm; tmp != NULL; tmp = tmp->next)
	{
		if (tmp->msg->to != msg->to)
			continue;
		if (tmp->msg->args.brnx_watch.type != msg->args.brnx_watch.type)
			continue;

		if (tmp->prev == NULL)
			next_alarm = tmp->next;
		else
			tmp->prev->next = tmp->next;
		if (tmp->next != NULL)
			tmp->next->prev = tmp->prev;

		msg_free((new_alarm = tmp)->msg);
		break;
	}

	/* If the requested alarm doesn't already exist, create it. */
	if (new_alarm == NULL)
		new_alarm = kmalloc(sizeof(alarm_t));

	/* (Re)insert the requested alarm into the list; keep the list sorted
	 * by time.  If the requested alarm belongs at the front of the list,
	 * insert it there.  Otherwise, find where it belongs and insert it
	 * there. */
	new_alarm->uptime = uptime_to_send;
	new_alarm->msg = msg;
	if (next_alarm == NULL || next_alarm->uptime > uptime_to_send)
	{
		new_alarm->prev = NULL;
		if ((new_alarm->next = next_alarm) != NULL)
			next_alarm->prev = new_alarm;
		next_alarm = new_alarm;
	}
	else
	{
		tmp = next_alarm;
		while (tmp->next != NULL && tmp->next->uptime <= uptime_to_send)
			tmp = tmp->next;
		new_alarm->prev = tmp;
		if ((new_alarm->next = tmp->next) != NULL)
			tmp->next->prev = new_alarm;
		tmp->next = new_alarm;
	}

	intr_unlock();
}

/*----------------------------------------------------------------------------*\
 |				timer_sigalrm()				      |
\*----------------------------------------------------------------------------*/
void timer_sigalrm(msg_t *msg)
{

/* Set a SIGALRM to be generated for the sending process after the specified
 * number of seconds. */

	clock_t uptime_to_send = uptime + msg->args.alarm.seconds * HZ;
	sigalrm_t *tmp;
	sigalrm_t *new_sigalrm = NULL;
	msg->args.alarm.ret_val = 0;

	intr_lock();

	/* If the sender already has a pending SIGALRM, remove it from the list.
	 * We'll either cancel or reinsert it later (to keep the list sorted by
	 * time). */
	for (tmp = next_sigalrm; tmp != NULL; tmp = tmp->next)
	{
		if (tmp->pid != msg->from)
			continue;

		if (tmp->prev == NULL)
			next_sigalrm = tmp->next;
		else
			tmp->prev->next = tmp->next;
		if (tmp->next != NULL)
			tmp->next->prev = tmp->prev;

		msg->args.alarm.ret_val = (tmp->uptime - uptime) / HZ;
		msg->args.alarm.ret_val += (tmp->uptime - uptime) % HZ > 0;
		new_sigalrm = tmp;
		break;
	}

	/* If the sender just wants its pending SIGALRM canceled, free it and
	 * return. */
	if (msg->args.alarm.seconds == 0)
	{
		kfree(new_sigalrm);
		intr_unlock();
		return;
	}

	/* If the sender doesn't have a pending SIGALRM, create one. */
	if (new_sigalrm == NULL)
		(new_sigalrm = kmalloc(sizeof(sigalrm_t)))->pid = msg->from;

	/* (Re)insert the SIGALRM into the list; keep the list sorted by time.
	 * If the SIGALRM belongs at the front of the list, insert it there.
	 * Otherwise, find where it belongs and insert it there. */
	new_sigalrm->uptime = uptime_to_send;
	if (next_sigalrm == NULL || next_sigalrm->uptime > uptime_to_send)
	{
		new_sigalrm->prev = NULL;
		if ((new_sigalrm->next = next_sigalrm) != NULL)
			next_sigalrm->prev = new_sigalrm;
		next_sigalrm = new_sigalrm;
	}
	else
	{
		tmp = next_sigalrm;
		while (tmp->next != NULL && tmp->next->uptime <= uptime_to_send)
			tmp = tmp->next;
		new_sigalrm->prev = tmp;
		if ((new_sigalrm->next = tmp->next) != NULL)
			tmp->next->prev = new_sigalrm;
		tmp->next = new_sigalrm;
	}

	intr_unlock();
}

/*----------------------------------------------------------------------------*\
 |				  timer_main()				      |
\*----------------------------------------------------------------------------*/
void timer_main(void)
{
	msg_t *msg;

	timer_init();
	while ((msg = msg_receive(ANYONE))->op != SHUTDOWN)
		switch (msg->op)
		{
			case SYS_ALARM:
				timer_sigalrm(msg);
				msg_reply(msg);
				break;
			case SYS_TIME:
				cmos_update_time(secs_pending);
				secs_pending = 0;
				if (msg->args.time.tloc != NULL)
					*msg->args.time.tloc = secs_since_epoch;
				msg->args.time.ret_val = secs_since_epoch;
				msg_reply(msg);
				break;
			case FREQUENCY:
				timer_freq(msg->args.brnx_freq.counter,
					   msg->args.brnx_freq.freq);
				msg_free(msg);
				break;
			case WATCHDOG:
				timer_watch(msg);
				/* Subtle: msg is neither sent nor freed --
				 * rather, it's queued to be sent later. */
				break;
			case UPTIME:
				msg->args.brnx_uptime.ret_val = uptime;
				msg_reply(msg);
				break;
			default:
				panic("timer_main", "unexpected message");
				msg_free(msg);
				break;
		}
	timer_deinit(msg);
}

/*----------------------------------------------------------------------------*\
 |				 timer_sleep()				      |
\*----------------------------------------------------------------------------*/
void timer_sleep(clock_t ticks)
{

/* Suspend the current thread's execution for the specified number of ticks. */

	clock_t uptime_to_wake = uptime + ticks;

	while (uptime < uptime_to_wake)
		proc_sched();
}
