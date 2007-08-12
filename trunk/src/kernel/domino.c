/*----------------------------------------------------------------------------*\
 |	domino.c							      |
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

#define BRAINIX
#include <kernel/kernel.h>

/*
 * each rally is associated with one msg_t.  The dominos grow
 * off of each rally storing first the pid of the sender, then
 * each pid that the message is being sent to.
 */

typedef struct domino
{
	pid_t pid;
	struct domino *prev;
	struct domino *next;
} domino_t;

/*
 * TODO: a heap would be nice here instead of doubly linked list.
 * 	(see how many times rally_find() is called from	calc_next_mid)?
 * 	array implementation using NUM_PROCS?
 */
typedef struct rally
{
	mid_t mid;
	unsigned char op;
	domino_t *domino;
	struct rally *prev;
	struct rally *next;
} rally_t;

/* Global variable: */
rally_t *rally;

/* Function prototypes: */
rally_t *rally_find(mid_t mid);
bool rally_exists(mid_t mid);
void rally_create(mid_t mid, unsigned char op);
void rally_grow(mid_t mid, pid_t from, pid_t to);
void rally_destroy(mid_t mid);
void rally_print(mid_t mid);

/*----------------------------------------------------------------------------*\
 |				  rally_find()				      |
\*----------------------------------------------------------------------------*/
rally_t *rally_find(mid_t mid)
{
	rally_t *r;

	for (r = rally; r != NULL && r->next != rally; r = r->next)
		if (r->mid == mid)
			break;
	return r == NULL || r->mid != mid ? NULL : r;
}

/*----------------------------------------------------------------------------*\
 |				 rally_exists()				      |
\*----------------------------------------------------------------------------*/
bool rally_exists(mid_t mid)
{
	return rally_find(mid) != NULL;
}

/*----------------------------------------------------------------------------*\
 |				 rally_create()				      |
\*----------------------------------------------------------------------------*/
void rally_create(mid_t mid, unsigned char op)
{
	rally_t *r;

	/* allocate a rally and insert onto front of 'rally' linked list
	 */
	(r = kmalloc(sizeof(rally_t)))->mid = mid;
	r->op = op;
	r->domino = NULL;
	r->prev = rally == NULL ? r : rally->prev;
	r->next = rally == NULL ? r : rally;
	if (rally == NULL)
		rally = r;
	rally->prev = rally->prev->next = r;
}

/*----------------------------------------------------------------------------*\
 |				  rally_grow()				      |
\*----------------------------------------------------------------------------*/
void rally_grow(mid_t mid, pid_t from, pid_t to)
{
	rally_t *r;
	domino_t *d;
	bool first;

	first = (r = rally_find(mid))->domino == NULL;
	(d = kmalloc(sizeof(domino_t)))->pid = first ? from : to;
	d->prev = first ? d : r->domino->prev;
	d->next = first ? d : r->domino;
	if (first)
	{
		/*
		 * first domino has a pid of the sending process.
		 * initialize this stub, and add a second domino
		 * with the process id of the receiver.
		 */
		r->domino = d;
		(d = kmalloc(sizeof(domino_t)))->pid = to;
		d->prev = (d->next = r->domino)->prev;
	}
	r->domino->prev = r->domino->prev->next = d;
#if DEBUG
	rally_print(mid);
#endif
}

/*----------------------------------------------------------------------------*\
 |				rally_destroy()				      |
\*----------------------------------------------------------------------------*/
void rally_destroy(mid_t mid)
{
	rally_t *r;
	domino_t *d;

#if DEBUG
	/*rally_print(mid);*/
#endif
	/*
	 * kfree all the dominos of this rally
	 */
	for (r = rally_find(mid); (d = r->domino) != NULL; )
	{
		if (d == r->domino)
			r->domino = d == d->next ? NULL : d->next;
		d->next->prev = (d->prev->next = d->next)->prev->prev;
		kfree(d);
	}
	/*
	 * remove r from rally linked list, and kfree
	 */
	if (r == rally)
		rally = r == r->next ? NULL : r->next;
	r->next->prev = (r->prev->next = r->next)->prev->prev;
	kfree(r);
}

/*----------------------------------------------------------------------------*\
 |				 rally_print()				      |
\*----------------------------------------------------------------------------*/
void rally_print(mid_t mid)
{
	rally_t *r = rally_find(mid);
	domino_t *d = r->domino;

	set_attr(SOLID, UNIMPORTANT_BG, UNIMPORTANT_FG);
	printf("%d(", mid);
	print_op(r->op);
	printf("):");
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf(" ");
	for (; d != r->domino->prev->prev; d = d->next)
	{
		set_attr(SOLID, UNIMPORTANT_BG, UNIMPORTANT_FG);
		print_pid(d->pid);
		set_attr(SOLID, NORMAL_BG, NORMAL_FG);
		printf(" ");
		set_attr(SOLID, UNIMPORTANT_BG, UNIMPORTANT_FG);
		printf("->");
		set_attr(SOLID, NORMAL_BG, NORMAL_FG);
		printf(" ");
	}
	print_pid(d->pid);
	printf(" ");
	set_attr(SOLID, UNIMPORTANT_BG, UNIMPORTANT_FG);
	printf("->");
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf(" ");
	print_pid((d = d->next)->pid);
	printf("\n");
}
