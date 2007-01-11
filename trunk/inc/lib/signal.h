/*----------------------------------------------------------------------------*\
 |	signal.h - signals						      |
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

#ifndef _SIGNAL_H
#define _SIGNAL_H

#define SIG_DFL		((void (*)(int)) 1) /* Request default sig handling.  */
#define SIG_ERR		((void (*)(int)) 2) /* Return value in case of error. */
#define SIG_HOLD	((void (*)(int)) 3) /* Request that signal be held.   */
#define SIG_IGN		((void (*)(int)) 4) /* Request that sig be ignored.   */

/* Used for process and process group IDs: */
#ifndef _PID_T
#define _PID_T
typedef long pid_t;
#endif

/* Used to represent sets of signals: */
typedef unsigned long sigset_t;

/* Used for user IDs: */
#ifndef _UID_T
#define _UID_T
typedef unsigned long uid_t;
#endif

#define SIGABRT		 0 /* Process abort signal.                           */
#define SIGALRM		 1 /* Alarm clock.                                    */
#define SIGBUS		 2 /* Access to an undefined portion of a mem object. */
#define SIGCHLD		 3 /* Child proc terminated, stopped, or continued.   */
#define SIGCONT		 4 /* Continue executing, if stopped.                 */
#define SIGFPE		 5 /* Erroneous arithmetic operation.                 */
#define SIGHUP		 6 /* Hangup.                                         */
#define SIGILL		 7 /* Illegal instruction.                            */
#define SIGINT		 8 /* Terminal interrupt signal.                      */
#define SIGKILL		 9 /* Kill (cannot be caught or ignored).             */
#define SIGPIPE		10 /* Write on a pipe with no one to read it.         */
#define SIGQUIT		11 /* Terminal quit signal.                           */
#define SIGSEGV		12 /* Invalid memory reference.                       */
#define SIGSTOP		13 /* Stop executing (cannot be caught or ignored).   */
#define SIGTERM		14 /* Termination signal.                             */
#define SIGTSTP		15 /* Terminal stop signal.                           */
#define SIGTTIN		16 /* Background process attempting read.             */
#define SIGTTOU		17 /* Background process attempting write.            */
#define SIGUSR1		18 /* User-defined signal 1.                          */
#define SIGUSR2		19 /* User-defined signal 2.                          */
#define SIGPOLL		20 /* Pollable event.                                 */
#define SIGPROF		21 /* Profiling timer expired.                        */
#define SIGSYS		22 /* Bad system call.                                */
#define SIGTRAP		23 /* Trace/breakpoint trap.                          */
#define SIGURG		24 /* High bandwidth data is available at a socket.   */
#define SIGVTALRM	25 /* Virtual timer expired.                          */
#define SIGXCPU		26 /* CPU time limit exceeded.                        */
#define SIGXFSZ		27 /* File size limit exceeded.                       */

typedef struct
{
	int si_signo;  /* Signal number.                   */
	int si_errno;  /* Signal error number.             */
	int si_code;   /* Signal code.                     */
	pid_t si_pid;  /* Process ID of sending process.   */
	uid_t si_uid;  /* Real user ID of sending process. */
	void *si_addr; /* Address of faulting instruction. */
	int si_status; /* Signal exit value.               */
	long si_band;  /* SIGPOLL band event.              */
} siginfo_t;

struct sigaction
{
	/* Pointer to a signal-catching function or one of the macros SIG_IGN or
	 * SIG_DFL: */
	void (*sa_handler)(int);

	/* Set of signals to be blocked during execution of the signal handling
	 * function: */
	sigset_t sa_mask;

	/* Special flags. */
	int sa_flags;

	/* Pointer to a signal-catching function: */
	void (*sa_sigaction)(int, siginfo_t *, void *);
};

#define SIG_BLOCK	1
#define SIG_UNBLOCK	2
#define SIG_SETMASK	3

/* Function prototypes: */
int kill(pid_t pid, int sig);
int sigaction(int sig, const struct sigaction *act, struct sigaction *oact);
int sigaddset(sigset_t *set, int signo);
int sigdelset(sigset_t *set, int signo);
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigismember(sigset_t *set, int signo);
int sigpending(sigset_t *set);
int sigprocmask(int how, const sigset_t *set, sigset_t *oset);
int sigsuspend(const sigset_t *sigmask);

#endif
