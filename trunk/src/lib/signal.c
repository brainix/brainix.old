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

#include <kernel/syscall.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

#define NUM_SIGS	(SIGXFSZ + 1)

/*----------------------------------------------------------------------------*\
 |				     kill()				      |
\*----------------------------------------------------------------------------*/
int kill(pid_t pid, int sig)
{

/* Send a signal to a process or a group of processes. */

	return syscall(SYS_KILL, pid, sig, 0);
}

/*----------------------------------------------------------------------------*\
 |				  sigaction()				      |
\*----------------------------------------------------------------------------*/
int sigaction(int sig, const struct sigaction *act, struct sigaction *oact)
{

/* Examine and change a signal action. */

	return syscall(SYS_SIGACTION,
		       sig,
		       (unsigned long) act,
		       (unsigned long) oact);
}

/*----------------------------------------------------------------------------*\
 |				  sigaddset()				      |
\*----------------------------------------------------------------------------*/
int sigaddset(sigset_t *set, int signo)
{

/* Add a signal to a signal set. */

	if (signo < 0 || signo >= NUM_SIGS)
	{
		errno = EINVAL;
		return -1;
	}
	*set |= 1 << signo;
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				  sigdelset()				      |
\*----------------------------------------------------------------------------*/
int sigdelset(sigset_t *set, int signo)
{

/* Delete a signal from a signal set. */

	if (signo < 0 || signo >= NUM_SIGS)
	{
		errno = EINVAL;
		return -1;
	}
	*set &= ~(1 << signo);
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				 sigemptyset()				      |
\*----------------------------------------------------------------------------*/
int sigemptyset(sigset_t *set)
{

/* Initialize and empty a signal set. */

	*set = 0;
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				  sigfillset()				      |
\*----------------------------------------------------------------------------*/
int sigfillset(sigset_t *set)
{

/* Initialize and fill a signal set. */

	*set = ~0;
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				 sigismember()				      |
\*----------------------------------------------------------------------------*/
int sigismember(sigset_t *set, int signo)
{

/* Test for a signal in a signal set. */

	if (signo < 0 || signo >= NUM_SIGS)
	{
		errno = EINVAL;
		return -1;
	}
	return *set >> signo & 1;
}

/*----------------------------------------------------------------------------*\
 |				  sigpending()				      |
\*----------------------------------------------------------------------------*/
int sigpending(sigset_t *set)
{

/* Examine pending signals. */

	return syscall(SYS_SIGPENDING, (unsigned long) set, 0, 0);
}

/*----------------------------------------------------------------------------*\
 |				 sigprocmask()				      |
\*----------------------------------------------------------------------------*/
int sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{

/* Examine and change blocked signals. */

	return syscall(SYS_SIGPROCMASK,
		       how,
		       (unsigned long) set,
		       (unsigned long) oset);
}

/*----------------------------------------------------------------------------*\
 |				  sigsuspend()				      |
\*----------------------------------------------------------------------------*/
int sigsuspend(const sigset_t *sigmask)
{

/* Wait for a signal. */

	sigset_t tmp;

	sigprocmask(SIG_SETMASK, sigmask, &tmp);
	pause();
	sigprocmask(SIG_SETMASK, &tmp, NULL);
	return -1;
}
