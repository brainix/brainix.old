/*----------------------------------------------------------------------------*\
 |	signal.c							      |
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

#define NUM_SIGS	(SIGXFSZ + 1)

#define TERM		0
#define IGN		1
#define STOP		2
#define CONT		3
#define CATCH		4

/* Global variables: */
sigset_t blocked[NUM_PROCS];
sigset_t pending[NUM_PROCS];
struct sigaction action[NUM_PROCS][NUM_SIGS];

/* Function prototypes: */
void sig_init(pid_t pid);
unsigned char sig_action(pid_t pid, int signo);
void sig_deliver(void);
bool sig_srch(pid_t from, pid_t to, pid_t pid);
bool sig_perm(pid_t from, pid_t to);
void sig_send(pid_t pid, int sig);
int do_kill(pid_t pid, int sig);
int do_pause(void);
int do_sigaction(int sig, const struct sigaction *act, struct sigaction *oact);
int do_sigpending(sigset_t *set);
int do_sigprocmask(int how, const sigset_t *set, sigset_t *oset);

/*----------------------------------------------------------------------------*\
 |				   sig_init()				      |
\*----------------------------------------------------------------------------*/
void sig_init(pid_t pid)
{
	int signo;

	sigemptyset(&blocked[pid]);
	sigemptyset(&pending[pid]);
	for (signo = 0; signo < NUM_SIGS; signo++)
	{
		action[pid][signo].sa_handler = SIG_DFL;
		sigemptyset(&action[pid][signo].sa_mask);
		action[pid][signo].sa_flags = 0;
	}
}

/*----------------------------------------------------------------------------*\
 |				  sig_action()				      |
\*----------------------------------------------------------------------------*/
unsigned char sig_action(pid_t pid, int signo)
{
	if (action[pid][signo].sa_handler == SIG_DFL)
		switch (signo)
		{
			case SIGCHLD : return IGN;
			case SIGURG  : return IGN;
			case SIGSTOP : return STOP;
			case SIGTSTP : return STOP;
			case SIGTTIN : return STOP;
			case SIGTTOU : return STOP;
			case SIGCONT : return CONT;
			default      : return TERM;
		}
	if (action[pid][signo].sa_handler == SIG_IGN)
		return IGN;
	return CATCH;
}

/*----------------------------------------------------------------------------*\
 |				 sig_deliver()				      |
\*----------------------------------------------------------------------------*/
void sig_deliver(void)
{
	pid_t pid = do_getpid();
	int signo;

	for (signo = 0; ~blocked[pid] & pending[pid]; signo++)
	{
		if (sigismember(&blocked[pid], signo))
			continue;
		if (!sigismember(&pending[pid], signo))
			continue;
		switch (sig_action(pid, signo))
		{
			case  TERM: do_exit(1);                           break;
			case   IGN:                                       break;
			case  STOP:                                       break;
			case  CONT:                                       break;
			case CATCH: action[pid][signo].sa_handler(signo); break;
		}
		sigdelset(&pending[pid], signo);
	}
}

/*----------------------------------------------------------------------------*\
 |				   sig_srch()				      |
\*----------------------------------------------------------------------------*/
bool sig_srch(pid_t from, pid_t to, pid_t pid)
{
	if ((proc_info(to, STATE) == NONEXIST)                          ||
	    (pid  >  0 && pid != to)                                    ||
	    (pid ==  0 && proc_info(from, PGRP) != proc_info(to, PGRP)) ||
	    (pid == -1 && to < INIT_PID)                                ||
	    (pid  < -1 && -pid != proc_info(to, PGRP)))
		return false;
	return true;
}

/*----------------------------------------------------------------------------*\
 |				   sig_perm()				      |
\*----------------------------------------------------------------------------*/
bool sig_perm(pid_t from, pid_t to)
{
	if (proc_info(from, EUID)                        &&
	    proc_info(from, UID)  != proc_info(to, UID)  &&
	    proc_info(from, UID)  != proc_info(to, EUID) &&
	    proc_info(from, EUID) != proc_info(to, UID)  &&
	    proc_info(from, EUID) != proc_info(to, EUID))
		return false;
	return true;
}

/*----------------------------------------------------------------------------*\
 |				   sig_send()				      |
\*----------------------------------------------------------------------------*/
void sig_send(pid_t pid, int sig)
{
	switch (sig)
	{
		case SIGSTOP:
		case SIGTSTP:
		case SIGTTIN:
		case SIGTTOU:
			sigdelset(&pending[pid], SIGCONT);
			break;
		case SIGCONT:
			sigdelset(&pending[pid], SIGSTOP);
			sigdelset(&pending[pid], SIGTSTP);
			sigdelset(&pending[pid], SIGTTIN);
			sigdelset(&pending[pid], SIGTTOU);
			proc_wakeup(pid);
			break;
	}
	if (sig_action(pid, sig) == IGN)
		return;
	sigaddset(&pending[pid], sig);
	if (~blocked[pid] & pending[pid])
		proc_wakeup(pid);
}

/*----------------------------------------------------------------------------*\
 |				   do_kill()				      |
\*----------------------------------------------------------------------------*/
int do_kill(pid_t pid, int sig)
{

/* Perform the kill() system call.  Send a signal to a process or group of
 * processes. */

	pid_t from = do_getpid();
	pid_t to;
	int err_code = ESRCH;

	if (sig < 0 || sig >= NUM_SIGS)
	{
		errno = EINVAL;
		return -1;
	}
	for (to = 0; to < NUM_PROCS; to++)
	{
		if (!sig_srch(from, to, pid))
			continue;
		if (!sig_perm(from, to))
		{
			if (err_code)
				err_code = EPERM;
			continue;
		}
		err_code = 0;
		sig_send(to, sig);
	}
	if (err_code)
		errno = err_code;
	return err_code ? -1 : 0;
}

/*----------------------------------------------------------------------------*\
 |				   do_pause()				      |
\*----------------------------------------------------------------------------*/
int do_pause(void)
{

/* Perform the pause() system call.  Suspend the thread until a signal is
 * received. */

	pid_t pid = do_getpid();

	while ((~blocked[pid] & pending[pid]) == 0)
		proc_sleep(pid);

	sig_deliver();
	errno = EINTR;
	return -1;
}

/*----------------------------------------------------------------------------*\
 |				 do_sigaction()				      |
\*----------------------------------------------------------------------------*/
int do_sigaction(int sig, const struct sigaction *act, struct sigaction *oact)
{

/* Perform the sigaction() system call.  Examine and change a signal action. */

	pid_t pid = do_getpid();

	if (sig < 0 || sig >= NUM_SIGS)
	{
		errno = EINVAL;
		return -1;
	}
	if (oact != NULL)
		*oact = action[pid][sig];
	if (act != NULL)
	{
		if (sig == SIGKILL || sig == SIGSTOP)
		{
			errno = EINVAL;
			return -1;
		}
		action[pid][sig] = *act;
	}
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				do_sigpending()				      |
\*----------------------------------------------------------------------------*/
int do_sigpending(sigset_t *set)
{

/* Perform the sigpending() system call.  Examine pending signals. */

	pid_t pid = do_getpid();

	*set = blocked[pid] & pending[pid];
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				do_sigprocmask()			      |
\*----------------------------------------------------------------------------*/
int do_sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{

/* Perform the sigprocmask() system call.  Examine and change blocked
 * signals. */

	pid_t pid = do_getpid();

	if (oset != NULL)
		*oset = blocked[pid];
	if (set != NULL)
	{
		switch (how)
		{
			case SIG_BLOCK   : blocked[pid] |= *set;  break;
			case SIG_UNBLOCK : blocked[pid] &= ~*set; break;
			case SIG_SETMASK : blocked[pid] = *set;   break;
			default          : errno = EINVAL;        return -1;
		}
		sigdelset(&blocked[pid], SIGKILL);
		sigdelset(&blocked[pid], SIGSTOP);
	}
	return 0;
}
