/*----------------------------------------------------------------------------*\
 |	process.c							      |
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

typedef struct proc
{
	gid_t egid; /* Effective group ID. */
	uid_t euid; /* Effective user ID.  */
	gid_t gid;  /* Real group ID.      */
	pid_t pgrp; /* Process group ID.   */
	pid_t pid;  /* Process ID.         */
	pid_t ppid; /* Parent process ID.  */
	uid_t uid;  /* Real user ID.       */

	clock_t utime;  /* User time.                                 */
	clock_t stime;  /* System time.                               */
	clock_t cutime; /* User time of terminated child processes.   */
	clock_t cstime; /* System time of terminated child processes. */

	unsigned char state;
	unsigned char kernel_time;
	struct proc *prev;
	struct proc *next;
} proc_t;

/* Global variables: */
proc_t proc[NUM_PROCS]; /* Process table.   */
proc_t *idle_proc;      /* Idle process.    */
proc_t *ready_q;        /* Ready queue.     */
proc_t *current_proc;   /* Current process. */

/* Function prototypes: */
void proc_init(void);
pid_t calc_next_pid(void);
void proc_enq(proc_t *proc_ptr);
void proc_deq(proc_t *proc_ptr, unsigned char state);
pid_t proc_create(unsigned long entry, bool kernel);
void proc_destroy(pid_t pid);
void proc_sleep(pid_t pid);
void proc_wakeup(pid_t pid);
void proc_sched(void);
void proc_fork(msg_t *msg);
void proc_exec(msg_t *msg);
void proc_exit(msg_t *msg);
id_t proc_info(pid_t pid, unsigned char request);
void proc_time(bool kernel);
void proc_update(void);
int proc_setpgrp(void);
clock_t do_times(struct tms *buffer);
void proc_nap(void);

/* Function prototypes in task.c: */
void task_init(void);
bool task_create(pid_t pid, unsigned long entry, bool kernel);
void task_destroy(pid_t pid);
void task_switch(pid_t pid);
void task_fork(pid_t pid_1, pid_t pid_2);
void task_exec(pid_t pid, unsigned long entry);

/*----------------------------------------------------------------------------*\
 |				  proc_init()				      |
\*----------------------------------------------------------------------------*/
void proc_init(void)
{

/* Initialize multi-processing. */

	pid_t pid;

	/* Initialize multi-tasking. */
	task_init();

	/* Initialize each process in the table. */
	for (pid = 0; pid < NUM_PROCS; pid++)
	{
		proc[pid].pid = pid;
		proc[pid].state = NONEXIST;
		proc[pid].next = proc[pid].prev = &proc[pid];
	}

	/* Fill in the process info required by POSIX. */
	idle_proc = &proc[IDLE_PID];
	idle_proc->egid = 0;
	idle_proc->euid = 0;
	idle_proc->gid = 0;
	idle_proc->pgrp = 0;
	idle_proc->pid = 0;
	idle_proc->ppid = 0;
	idle_proc->uid = 0;
	idle_proc->utime = 0;
	idle_proc->stime = 0;
	idle_proc->cutime = 0;
	idle_proc->cstime = 0;

	/* Mark the process ready and initialize the ready queue. */
	idle_proc->state = READY;
	idle_proc->kernel_time = 0;
	ready_q = NULL;
	current_proc = idle_proc;
	sig_init(IDLE_PID);
}

/*----------------------------------------------------------------------------*\
 |				calc_next_pid()				      |
\*----------------------------------------------------------------------------*/
pid_t calc_next_pid(void)
{

/* Calculate the next PID - claim a slot in the process table. */

	static pid_t pid = IDLE_PID;
	pid_t bookmark = pid;

	do
		if ((pid = (pid + 1) % NUM_PROCS) == bookmark)
			if (proc[pid].state != NONEXIST)
				return -1;
	while (proc[pid].state != NONEXIST);
	return pid;
}

/*----------------------------------------------------------------------------*\
 |				   proc_enq()				      |
\*----------------------------------------------------------------------------*/
void proc_enq(proc_t *proc_ptr)
{

/* Add a process to the ready queue. */

	proc_ptr->state = READY;
	if (ready_q == NULL)
		ready_q = proc_ptr->next = proc_ptr->prev = proc_ptr;
	else
	{
		proc_ptr->prev = (proc_ptr->next = ready_q)->prev;
		ready_q->prev = ready_q->prev->next = proc_ptr;
	}
}

/*----------------------------------------------------------------------------*\
 |				   proc_deq()				      |
\*----------------------------------------------------------------------------*/
void proc_deq(proc_t *proc_ptr, unsigned char state)
{

/* Remove a process from the ready queue. */

	proc_ptr->state = state;
	if (ready_q == proc_ptr)
		ready_q = proc_ptr == proc_ptr->next ? NULL : proc_ptr->next;
	proc_ptr->prev->next = proc_ptr->next;
	proc_ptr->next->prev = proc_ptr->prev;
	proc_ptr->next = proc_ptr->prev = proc_ptr;
}

/*----------------------------------------------------------------------------*\
 |				 proc_create()				      |
\*----------------------------------------------------------------------------*/
pid_t proc_create(unsigned long entry, bool kernel)
{

/* Create a process. */

	pid_t pid;
	proc_t *proc_ptr;

	intr_lock();

	/* Claim a slot in the process table. */
	if ((pid = calc_next_pid()) == -1 || !task_create(pid, entry, kernel))
	{
		/* There's no free slot in the process table. */
		intr_unlock();
		return -1;
	}

	/* Fill in the process info required by POSIX. */
	proc_ptr = &proc[pid];
	proc_ptr->egid = proc[current_proc->pid].egid;
	proc_ptr->euid = proc[current_proc->pid].euid;
	proc_ptr->gid = proc[current_proc->pid].gid;
	proc_ptr->pgrp = proc[current_proc->pid].pgrp;
	proc_ptr->pid = pid;
	proc_ptr->ppid = proc[current_proc->pid].pid;
	proc_ptr->uid = proc[current_proc->pid].uid;
	proc_ptr->utime = 0;
	proc_ptr->stime = 0;
	proc_ptr->cutime = 0;
	proc_ptr->cstime = 0;

	/* Add the process to the ready queue. */
	proc_ptr->kernel_time = 0;
	proc_enq(proc_ptr);
	sig_init(pid);

	intr_unlock();
	return pid;
}

/*----------------------------------------------------------------------------*\
 |				 proc_destroy()				      |
\*----------------------------------------------------------------------------*/
void proc_destroy(pid_t pid)
{

/* Destroy a process. */

	proc_t *proc_ptr = &proc[pid];
	pid_t j;

	intr_lock();

	/* Check for special cases. */
	if (proc_ptr->state == NONEXIST)
	{
		/* A nonexistent process is being destroyed.  There's nothing to
		 * do. */
		intr_unlock();
		return;
	}

	task_destroy(pid);

	if (proc_ptr->pgrp == pid)
		for (j = 0; j < NUM_PROCS; j++)
			if (proc[j].pgrp == pid)
				proc[j].pgrp = 0;

	/* Remove the process from the ready queue. */
	proc_deq(proc_ptr, NONEXIST);

	intr_unlock();
}

/*----------------------------------------------------------------------------*\
 |				  proc_sleep()				      |
\*----------------------------------------------------------------------------*/
void proc_sleep(pid_t pid)
{

/* Put a process to sleep. */

	proc_t *proc_ptr = &proc[pid];

	intr_lock();

	/* Check for special cases. */
	if (proc_ptr->state != READY)
	{
		/* A nonexistent, blocked, or zombie  process is being put to
		 * sleep.  There's nothing to do. */
		intr_unlock();
		return;
	}

	/* Remove the process from the ready queue. */
	proc_deq(proc_ptr, BLOCKED);

	intr_unlock();
	if (proc_ptr == current_proc)
		/* Oops.  The sleeping process is the one that's running now.
		 * Schedule another process. */
		proc_sched();
}

/*----------------------------------------------------------------------------*\
 |				 proc_wakeup()				      |
\*----------------------------------------------------------------------------*/
void proc_wakeup(pid_t pid)
{

/* Wake a process up. */

	proc_t *proc_ptr = &proc[pid];

	intr_lock();

	/* Check for special cases. */
	if (proc_ptr->state != BLOCKED)
	{
		/* A nonexistent, ready, or zombie process is being woken.
		 * There's nothing to do. */
		intr_unlock();
		return;
	}

	/* Add the process to the ready queue. */
	proc_enq(proc_ptr);

	intr_unlock();
	if (proc_ptr == proc_ptr->next)
		/* Oops.  The waking process is the only one in the ready queue.
		 * Schedule it now. */
		proc_sched();
}

/*----------------------------------------------------------------------------*\
 |				  proc_sched()				      |
\*----------------------------------------------------------------------------*/
void proc_sched(void)
{

/* This function is the process scheduler.  It's called when the current
 * process' timeslice has expired or when the current process voluntarily
 * relinquishes the CPU.  Pick the next process to run. */

	proc_t *proc_ptr;

	if (ready_q == NULL)
		/* There are no processes in the ready queue.  Pick the idle
		 * loop. */
		proc_ptr = idle_proc;
	else if (current_proc == idle_proc || current_proc->state == BLOCKED)
		/* We're currently running the idle loop but another process has
		 * become ready, or the current process has put itself to sleep
		 * (and removed itself from the ready queue).  Pick the first
		 * process in the ready queue. */
		proc_ptr = ready_q;
	else
		/* The current process' timeslice has expired.  Pick the next
		 * process in the ready queue. */
		proc_ptr = ready_q = ready_q->next;

	if (proc_ptr == current_proc)
		/* Oops.  The process that we've picked to run next is the one
		 * that's running now.  Save a context switch; just return. */
		return;
	else
		task_switch((current_proc = proc_ptr)->pid);
}

/*----------------------------------------------------------------------------*\
 |				  proc_fork()				      |
\*----------------------------------------------------------------------------*/
void proc_fork(msg_t *msg)
{

/* Perform the process manager's part of the fork() system call. */

	pid_t pid_1, pid_2;
	msg_t *m;

	pid_1 = msg->from;
	pid_2 = proc_create(0, USER);
	task_fork(pid_1, pid_2);
	msg->args.fork.ret_val = pid_2;
	m = msg_alloc(pid_2, SYS_FORK);
	m->args.fork.ret_val = 0;
	msg_send(m);
}

/*----------------------------------------------------------------------------*\
 |				  proc_exec()				      |
\*----------------------------------------------------------------------------*/
void proc_exec(msg_t *msg)
{

/* Perform the process manager's part of the exec() system call. */

	task_exec(msg->args.exec.pid, msg->args.exec.entry);
	proc_wakeup(msg->args.exec.pid);
}

/*----------------------------------------------------------------------------*\
 |				  proc_exit()				      |
\*----------------------------------------------------------------------------*/
void proc_exit(msg_t *msg)
{

/* Perform the process manager's part of the exit() system call. */

	proc_destroy(msg->from);
	msg_empty(msg->from);
}

/*----------------------------------------------------------------------------*\
 |				  proc_info()				      |
\*----------------------------------------------------------------------------*/
id_t proc_info(pid_t pid, unsigned char request)
{
	if (pid == CURRENT_TASK)
		pid = current_proc->pid;

	switch (request)
	{
		case   EGID: return proc[pid].egid;
		case   EUID: return proc[pid].euid;
		case    GID: return proc[pid].gid;
		case   PGRP: return proc[pid].pgrp;
		case    PID: return proc[pid].pid;
		case   PPID: return proc[pid].ppid;
		case    UID: return proc[pid].uid;
		case  UTIME: return proc[pid].utime;
		case  STIME: return proc[pid].stime;
		case CUTIME: return proc[pid].cutime;
		case CSTIME: return proc[pid].cstime;
		case  STATE: return proc[pid].state;
	}
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				  proc_time()				      |
\*----------------------------------------------------------------------------*/
void proc_time(bool kernel)
{
	current_proc->kernel_time += kernel ? 1 : -1;
}

/*----------------------------------------------------------------------------*\
 |				 proc_update()				      |
\*----------------------------------------------------------------------------*/
void proc_update(void)
{
	if (current_proc->kernel_time)
		current_proc->stime++;
	else
		current_proc->utime++;
}

/*----------------------------------------------------------------------------*\
 |				 proc_setpgrp()				      |
\*----------------------------------------------------------------------------*/
int proc_setpgrp(void)
{
	return proc[current_proc->pid].pgrp = current_proc->pid;
}

/*----------------------------------------------------------------------------*\
 |				   do_times()				      |
\*----------------------------------------------------------------------------*/
clock_t do_times(struct tms *buffer)
{

/* Perform the times() system call.  Get process and waited-for child process
 * times. */

	msg_t *msg;
	clock_t ret_val;

	if ((unsigned long) buffer < pg_addr(USER, HEAP, BOTTOM))
		return -EFAULT;

	buffer->tms_utime = proc[current_proc->pid].utime;
	buffer->tms_stime = proc[current_proc->pid].stime;
	buffer->tms_cutime = proc[current_proc->pid].cutime;
	buffer->tms_cstime = proc[current_proc->pid].cstime;

	msg = msg_alloc(TMR_PID, UPTIME);
	msg_send(msg);

	msg = msg_receive(TMR_PID);
	ret_val = msg->args.brnx_uptime.ret_val;
	msg_free(msg);

	return ret_val;
}

/*----------------------------------------------------------------------------*\
 |				   proc_nap()				      |
\*----------------------------------------------------------------------------*/
void proc_nap(void)
{
	proc_sleep(current_proc->pid);
}
