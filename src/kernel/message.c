/*----------------------------------------------------------------------------*\
 |	message.c							      |
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

#define BRAINIX
#include <kernel/kernel.h>

/* Global variable: */
msg_t *mbox[NUM_PROCS];

/* Function prototypes: */
void msg_init(void);
mid_t calc_next_mid(void);
msg_t *msg_alloc(pid_t to, unsigned char op);
void msg_free(msg_t *msg);
msg_t *msg_check(pid_t from);
msg_t *msg_receive(pid_t from);
void msg_send(msg_t *msg);
void msg_reply(msg_t *msg);
void msg_empty(pid_t pid);

/* Function prototypes in domino.c: */
bool rally_exists(mid_t mid);
void rally_create(mid_t mid, unsigned char op);
void rally_grow(mid_t mid, pid_t from, pid_t to);
void rally_destroy(mid_t mid);

/*----------------------------------------------------------------------------*\
 |				   msg_init()				      |
\*----------------------------------------------------------------------------*/
void msg_init(void)
{
	pid_t pid;

	for (pid = 0; pid < NUM_PROCS; pid++)
		mbox[pid] = NULL;
}

/*----------------------------------------------------------------------------*\
 |				calc_next_mid()				      |
\*----------------------------------------------------------------------------*/
mid_t calc_next_mid(void)
{

/* Calculate the next MID. */

	static mid_t mid = -1;
	mid_t bookmark = mid;

	do
		if ((mid = (mid + 1) % LONG_MAX) == bookmark)
			if (rally_exists(mid))
				panic("calc_next_mid", "out of MIDs");
	while (rally_exists(mid));
	return mid;
}

/*----------------------------------------------------------------------------*\
 |				  msg_alloc()				      |
\*----------------------------------------------------------------------------*/
msg_t *msg_alloc(pid_t to, unsigned char op)
{
	msg_t *msg;

	intr_lock();
	msg = kmalloc(sizeof(msg_t));
	msg->mid = calc_next_mid();
	msg->from = do_getpid();
	msg->to = to;
	msg->op = op;
	msg->next = msg->prev = msg;
	rally_create(msg->mid, msg->op);
	intr_unlock();
	return msg;
}

/*----------------------------------------------------------------------------*\
 |				   msg_free()				      |
\*----------------------------------------------------------------------------*/
void msg_free(msg_t *msg)
{
	intr_lock();
	if (msg == mbox[msg->to])
		mbox[msg->to] = msg == msg->next ? NULL : msg->next;
	msg->prev->next = msg->next;
	msg->next->prev = msg->prev;
	rally_destroy(msg->mid);
	kfree(msg);
	intr_unlock();
}

/*----------------------------------------------------------------------------*\
 |				  msg_check()				      |
\*----------------------------------------------------------------------------*/
msg_t *msg_check(pid_t from)
{

/* Check for a message from the specified sender.  If one exists, remove it
 * from the mailbox and return a pointer to it.  Otherwise, return NULL. */

	pid_t to = do_getpid();
	msg_t *msg = mbox[to];

	intr_lock();
	while (from != ANYONE && msg != NULL && msg->from != from)
		if ((msg = msg->next) == mbox[to])
			msg = NULL;
	if (msg != NULL)
	{
		if (msg == mbox[to])
			mbox[to] = msg == msg->next ? NULL : msg->next;
		msg->prev->next = msg->next;
		msg->next->prev = msg->prev;
	}
	intr_unlock();
	return msg;
}

/*----------------------------------------------------------------------------*\
 |				 msg_receive()				      |
\*----------------------------------------------------------------------------*/
msg_t *msg_receive(pid_t from)
{

/* Receive a message from the specified sender. */

	msg_t *msg;
	pid_t to = do_getpid();

	while ((msg = msg_check(from)) == NULL)
		/* Race condition! */
		proc_sleep(to);
	return msg;

	/*
	 | There's a race condition here.  Pretend you're a process and consider
	 | this possible sequence of events:
	 |
	 |     1. You check and there's no queued message.
	 |     2. An interrupt occurs and queues a message.
	 |     3. You go to sleep.
	 |
	 | In this case, there's a queued message but you still go to sleep.
	 | And only upon the arrival of the next message (which could happen a
	 | thousand hours from now) will you be woken.  This is potentially
	 | catastrophic and must be fixed before Brainix is used for anything
	 | important.
	 */
}

/*----------------------------------------------------------------------------*\
 |				   msg_send()				      |
\*----------------------------------------------------------------------------*/
void msg_send(msg_t *msg)
{

/* Send a message. */

	intr_lock();
	if (proc_info(msg->to, STATE) == NONEXIST)
	{
		msg_free(msg);
		panic("msg_send", "unknown recipient");
	}
	else
	{
		if (mbox[msg->to] == NULL)
			mbox[msg->to] = msg->next = msg->prev = msg;
		else
		{
			msg->prev = (msg->next = mbox[msg->to])->prev;
			mbox[msg->to]->prev = mbox[msg->to]->prev->next = msg;
		}
		rally_grow(msg->mid, msg->from, msg->to);
	}
	intr_unlock();
	proc_wakeup(msg->to);
}

/*----------------------------------------------------------------------------*\
 |				  msg_reply()				      |
\*----------------------------------------------------------------------------*/
void msg_reply(msg_t *msg)
{

/* Reply to a message. */

	/* Swap the from and to fields. */
	msg->to ^= msg->from;
	msg->from ^= msg->to;
	msg->to ^= msg->from;

	/* Send the message. */
	msg_send(msg);
}

/*----------------------------------------------------------------------------*\
 |				  msg_empty()				      |
\*----------------------------------------------------------------------------*/
void msg_empty(pid_t pid)
{
	while (mbox[pid] != NULL)
		msg_free(mbox[pid]);
}
