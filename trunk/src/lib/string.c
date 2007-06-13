/*----------------------------------------------------------------------------*\
 |	string.c							      |
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

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>

/*----------------------------------------------------------------------------*\
 |				    memchr()				      |
\*----------------------------------------------------------------------------*/
void *memchr(const void *s, int c, size_t n)
{

/* Find byte in memory. */

	const unsigned char *p = s;
	c = (unsigned char) c;

	for (; n; n--, p++)
	{
		if (*p != c)
			continue;
		return (void *) p;
	}
	return NULL;
}

/*----------------------------------------------------------------------------*\
 |				    memcmp()				      |
\*----------------------------------------------------------------------------*/
int memcmp(const void *s1, const void *s2, size_t n)
{

/* Compare bytes in memory. */

	const unsigned char *p1 = s1;
	const unsigned char *p2 = s2;

	for (; n; n--, p1++, p2++)
	{
		if (*p1 == *p2)
			continue;
		return *p1 - *p2;
	}
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				    memcpy()				      |
\*----------------------------------------------------------------------------*/
void *memcpy(void *s1, const void *s2, size_t n)
{
#if DEBUG
	printf("string.memcpy(): copying %s to %s.\n",s1,s2);
#endif
/* Copy bytes in memory. */

	char *p1 = s1;
	const char *p2 = s2;

	for (; n; n--, p1++, p2++)
		*p1 = *p2;
	return s1;
}

/*----------------------------------------------------------------------------*\
 |				   memmove()				      |
\*----------------------------------------------------------------------------*/
void *memmove(void *s1, const void *s2, size_t n)
{

/* Copy bytes in memory with overlapping areas. */

	char *p1 = s1;
	const char *p2 = s2;
	int over = p2 <= p1 && p2 + n > p1 ? -1 : 1;

	if (over == -1)
	{
		p1 += n - 1;
		p2 += n - 1;
	}
	for (; n; n--, p1 += over, p2 += over)
		*p1 = *p2;
	return s1;
}

/*----------------------------------------------------------------------------*\
 |				    memset()				      |
\*----------------------------------------------------------------------------*/
void *memset(void *s, int c, size_t n)
{

/* Set bytes in memory. */

	unsigned char *p = s;
	c = (unsigned char) c;

	for (; n; n--, p++)
		*p = c;
	return s;
}

/*----------------------------------------------------------------------------*\
 |				    strcat()				      |
\*----------------------------------------------------------------------------*/
char *strcat(char *s1, const char *s2)
{

/* Concatenate two strings. */

	char *p1 = s1;
	const char *p2 = s2;

	for (; *p1; p1++)
		;
	for (; (*p1 = *p2); p1++, p2++)
		;
	return s1;
}

/*----------------------------------------------------------------------------*\
 |				    strchr()				      |
\*----------------------------------------------------------------------------*/
char *strchr(const char *s, int c)
{

/* String scanning operation. */

	const char *p = s;
	char *ret_val = NULL;
	c = (char) c;

	for (; *p && ret_val == NULL; p++)
		if (*p == c)
			ret_val = (char *) p;
	return ret_val;
}

/*----------------------------------------------------------------------------*\
 |				    strcmp()				      |
\*----------------------------------------------------------------------------*/
int strcmp(const char *s1, const char *s2)
{

/* Compare two strings. */

	const unsigned char *p1 = (unsigned char *) s1;
	const unsigned char *p2 = (unsigned char *) s2;

	for (; *p1 && *p2 && *p1 == *p2; p1++, p2++)
		;
	if (*p1 == '\0' && *p2 != '\0')
		return -1;
	if (*p1 != '\0' && *p2 == '\0')
		return 1;
	return *p1 - *p2;
}

/*----------------------------------------------------------------------------*\
 |				   strcoll()				      |
\*----------------------------------------------------------------------------*/
int strcoll(const char *s1, const char *s2)
{

/* String comparison using collating information. */

	const char *p1 = s1;
	const char *p2 = s2;

	for (; *p1 == *p2; p1++, p2++)
		if (*p1 == '\0')
			return 0;
	return *p1 - *p2;
}

/*----------------------------------------------------------------------------*\
 |				    strcpy()				      |
\*----------------------------------------------------------------------------*/
char *strcpy(char *s1, const char *s2)
{

/* Copy a string. */

	char *p1 = s1;
	const char *p2 = s2;

	for (; (*p1 = *p2); p1++, p2++)
		;
	return s1;
}

/*----------------------------------------------------------------------------*\
 |				   strcspn()				      |
\*----------------------------------------------------------------------------*/
size_t strcspn(const char *s1, const char *s2)
{

/* Get the length of a complimentary substring. */

	const char *p1 = s1;
	const char *p2 = s2;

	for (; *p1; p1++)
		for (; *p2 != *p1 && *p2; p2++)
			if (*p2)
				break;
	return p1 - s1;
}

/*----------------------------------------------------------------------------*\
 |	      The following array maps error numbers to messages:	      |
\*----------------------------------------------------------------------------*/
static char *sys_errlist[] =
{
/*                 */	"",
/* E2BIG           */	"argument list too long",
/* EACCES          */	"permission denied",
/* EADDRINUSE      */	"address in use",
/* EADDRNOTAVAIL   */	"address not available",
/* EAFNOSUPPORT    */	"address family not supported",
/* EAGAIN          */	"resource unavailable, try again",
/* EALREADY        */	"connection already in progress",
/* EBADF           */	"bad file descriptor",
/* EBADMSG         */	"bad message",
/* EBUSY           */	"device or resource busy",
/* ECANCELED       */	"operation canceled",
/* ECHILD          */	"no child process",
/* ECONNABORTED    */	"connection aborted",
/* ECONNREFUSED    */	"connection refused",
/* ECONNRESET      */	"connection reset",
/* EDEADLK         */	"resource deadlock would occur",
/* EDESTADDRREQ    */	"destination address required",
/* EDOM            */	"math argument out of domain of function",
/* EDQUOT          */	"reserved",
/* EEXIST          */	"file exists",
/* EFAULT          */	"bad address",
/* EFBIG           */	"file too large",
/* EHOSTUNREACH    */	"host is unreachable",
/* EIDRM           */	"identifier removed",
/* EILSEQ          */	"illegal byte sequence",
/* EINPROGRESS     */	"operation in progress",
/* EINTR           */	"interrupted function",
/* EINVAL          */	"invalid argument",
/* EIO             */	"I/O error",
/* EISCONN         */	"socket is connected",
/* EISDIR          */	"is a directory",
/* ELOOP           */	"too many levels of symbolic links",
/* EMFILE          */	"too many open files",
/* EMLINK          */	"too many links",
/* EMSGSIZE        */	"message too large",
/* EMULTIHOP       */	"reserved",
/* ENAMETOOLONG    */	"file name too long",
/* ENETDOWN        */	"network is down",
/* ENETRESET       */	"connection aborted by network",
/* ENETUNREACH     */	"network unreachable",
/* ENFILE          */	"too many files open in system",
/* ENOBUFS         */	"no buffer space available",
/* ENODEV          */	"no such device",
/* ENOENT          */	"no such file or directory",
/* ENOEXEC         */	"executable file format error",
/* ENOLCK          */	"no locks available",
/* ENOLINK         */	"reserved",
/* ENOMEM          */	"not enough space",
/* ENOMSG          */	"no message of the desired type",
/* ENOPROTOOPT     */	"protocol not available",
/* ENOSPC          */	"no space left on device",
/* ENOSYS          */	"function not supported",
/* ENOTCONN        */	"the socket is not connected",
/* ENOTDIR         */	"not a directory",
/* ENOTEMPTY       */	"directory not empty",
/* ENOTSOCK        */	"not a socket",
/* ENOTSUP         */	"not supported",
/* ENOTTY          */	"inappropriate I/O control operation",
/* ENXIO           */	"no such device or address",
/* EOPNOTSUPP      */	"operation not supported on socket",
/* EOVERFLOW       */	"value too large to be stored in data type",
/* EPERM           */	"operation not permitted",
/* EPIPE           */	"broken pipe",
/* EPROTO          */	"protocol error",
/* EPROTONOSUPPORT */	"protocol not supported",
/* EPROTOTYPE      */	"protocol wrong type for socket",
/* ERANGE          */	"result too large",
/* EROFS           */	"read-only file system",
/* ESPIPE          */	"invalid seek",
/* ESRCH           */	"no such process",
/* ESTALE          */	"reserved",
/* ETIMEDOUT       */	"connection timed out",
/* ETXTBSY         */	"text file busy",
/* EXDEV           */	"cross-device link"
};

int sys_nerr = EXDEV + 1;

/*----------------------------------------------------------------------------*\
 |				   strerror()				      |
\*----------------------------------------------------------------------------*/
char *strerror(int errnum)
{

/* Get error message string. */

	if (errnum <= 0 || errnum >= sys_nerr)
		return "unknown error";
	return sys_errlist[errnum];
}

/*----------------------------------------------------------------------------*\
 |				    strlen()				      |
\*----------------------------------------------------------------------------*/
size_t strlen(const char *s)
{

/* Get string length. */

	const char *p = s;

	for (; *p; p++)
		;
	return p - s;
}

/*----------------------------------------------------------------------------*\
 |				   strncat()				      |
\*----------------------------------------------------------------------------*/
char *strncat(char *s1, const char *s2, size_t n)
{

/* Concatenate a string with part of another. */

	char *p1 = s1;
	const char *p2 = s2;

	for (; *p1; p1++)
		;
	for (; n && (*p1 = *p2); n--, p1++, p2++)
		;
	if (n == 0)
		*p1 = '\0';
	return s1;
}

/*----------------------------------------------------------------------------*\
 |				   strncmp()				      |
\*----------------------------------------------------------------------------*/
int strncmp(const char *s1, const char *s2, size_t n)
{

/* Compare part of two strings. */

	const char *p1 = s1;
	const char *p2 = s2;

	for (; n; n--, p1++, p2++)
	{
		if (*p1 != *p2)
		{
			if (*p1 == '\0' || *p2 == '\0')
				return *p1 == '\0' ? -1 : 1;
			return *p1 - *p2;
		}
		if (*p1 == '\0')
			return 0;
	}
	return 0;
}

/*----------------------------------------------------------------------------*\
 |				   strncpy()				      |
\*----------------------------------------------------------------------------*/
char *strncpy(char *s1, const char *s2, size_t n)
{

/* Copy part of a string. */

	char *p1 = s1;
	const char *p2 = s2;

	for (; n && *p2; n--, p1++, p2++)
		*p1 = *p2;
	for (; n; n--, p1++)
		*p1 = '\0';
	return s1;
}

/*----------------------------------------------------------------------------*\
 |				   strpbrk()				      |
\*----------------------------------------------------------------------------*/
char *strpbrk(const char *s1, const char *s2)
{

/* Scan a string for a byte. */

	const char *p1 = s1;
	const char *p2 = s2;

	for (p1 = s1; *p1; p1++)
	{
		for (p2 = s2; *p2 && *p2 != *p1; *p2++)
			;
		if (*p2)
			return (char *) p1;
	}
	return NULL;
}

/*----------------------------------------------------------------------------*\
 |				   strrchr()				      |
\*----------------------------------------------------------------------------*/
char *strrchr(const char *s, int c)
{

/* String scanning operation. */

	const char *p = s;
	char *ret_val = NULL;
	c = (char) c;

	for (; *p; p++)
		if (*p == c)
			ret_val = (char *) p;
	return ret_val;
}

/*----------------------------------------------------------------------------*\
 |	    The following array maps signal numbers to descriptions:	      |
\*----------------------------------------------------------------------------*/
static char *sys_siglist[] =
{
	"SIGABRT",
	"SIGALRM",
	"SIGBUS",
	"SIGCHLD",
	"SIGCONT",
	"SIGFPE",
	"SIGHUP",
	"SIGILL",
	"SIGINT",
	"SIGKILL",
	"SIGPIPE",
	"SIGQUIT",
	"SIGSEGV",
	"SIGSTOP",
	"SIGTERM",
	"SIGTSTP",
	"SIGTTIN",
	"SIGTTOU",
	"SIGUSR1",
	"SIGUSR2",
	"SIGPOLL",
	"SIGPROF",
	"SIGSYS",
	"SIGTRAP",
	"SIGURG",
	"SIGVTALRM",
	"SIGXCPU",
	"SIGXFSZ"
};

/*----------------------------------------------------------------------------*\
 |				  strsignal()				      |
\*----------------------------------------------------------------------------*/
char *strsignal(int sig)
{
	if (sig < SIGABRT || sig > SIGXFSZ)
		return "unknown signal";
	return sys_siglist[sig];
}

/*----------------------------------------------------------------------------*\
 |				    strspn()				      |
\*----------------------------------------------------------------------------*/
size_t strspn(const char *s1, const char *s2)
{

/* Get length of a substring. */

	const char *p1 = s1;
	const char *p2 = s2;

	for (p1 = s1; *p1; p1++)
	{
		/*
		 | Here, the original line was:
		 |
		 |	for (p2 = s2; *p2 && *p2 != *p1; p2++)
		 |
		 | This should've worked, but GCC bitched and moaned about the
		 | pointer arithmetic.  Thanks for the fix, Jagtesh.
		 */
		for (p2 = s2; *p2 && *p2 != *p1; *p2++)
			;
		if (*p2 == '\0')
			break;
	}
	return p1 - s1;
}

/*----------------------------------------------------------------------------*\
 |				    strstr()				      |
\*----------------------------------------------------------------------------*/
char *strstr(const char *s1, const char *s2)
{

/* Find a substring. */

	const char *p1 = s1;
	const char *p2 = s2;

	for (; strncmp(p1, p2, strlen(p2)); p1++)
		;
	return *p1 == '\0' ? NULL : (char *) p1;
}
