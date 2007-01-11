/*----------------------------------------------------------------------------*\
 |	errno.h - system error numbers					      |
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

#ifndef _ERRNO_H
#define _ERRNO_H

#define E2BIG		 1     /* Argument list too long.                    */
#define EACCES		 2     /* Permission denied.                         */
#define EADDRINUSE	 3     /* Address in use.                            */
#define EADDRNOTAVAIL	 4     /* Address not available.                     */
#define EAFNOSUPPORT	 5     /* Address family not supported.              */
#define EAGAIN		 6     /* Resource unavailable, try again.           */
#define EALREADY	 7     /* Connection already in progress.            */
#define EBADF		 8     /* Bad file descriptor.                       */
#define EBADMSG		 9     /* Bad message.                               */
#define EBUSY		10     /* Device or resource busy.                   */
#define ECANCELED	11     /* Operation canceled.                        */
#define ECHILD		12     /* No child process.                          */
#define ECONNABORTED	13     /* Connection aborted.                        */
#define ECONNREFUSED	14     /* Connection refused.                        */
#define ECONNRESET	15     /* Connection reset.                          */
#define EDEADLK		16     /* Resource deadlock would occur.             */
#define EDESTADDRREQ	17     /* Destination address required.              */
#define EDOM		18     /* Math argument out of domain of function.   */
#define EDQUOT		19     /* Reserved.                                  */
#define EEXIST		20     /* File exists.                               */
#define EFAULT		21     /* Bad address.                               */
#define EFBIG		22     /* File too large.                            */
#define EHOSTUNREACH	23     /* Host is unreachable.                       */
#define EIDRM		24     /* Identifier removed.                        */
#define EILSEQ		25     /* Illegal byte sequence.                     */
#define EINPROGRESS	26     /* Operation in progress.                     */
#define EINTR		27     /* Interrupted function.                      */
#define EINVAL		28     /* Invalid argument.                          */
#define EIO		29     /* I/O error.                                 */
#define EISCONN		30     /* Socket is connected.                       */
#define EISDIR		31     /* Is a directory.                            */
#define ELOOP		32     /* Too many levels of symbolic links.         */
#define EMFILE		33     /* Too many open files.                       */
#define EMLINK		34     /* Too many links.                            */
#define EMSGSIZE	35     /* Message too large.                         */
#define EMULTIHOP	36     /* Reserved.                                  */
#define ENAMETOOLONG	37     /* File name too long.                        */
#define ENETDOWN	38     /* Network is down.                           */
#define ENETRESET	39     /* Connection aborted by network.             */
#define ENETUNREACH	40     /* Network unreachable.                       */
#define ENFILE		41     /* Too many files open in system.             */
#define ENOBUFS		42     /* No buffer space available.                 */
#define ENODEV		43     /* No such device.                            */
#define ENOENT		44     /* No such file or directory.                 */
#define ENOEXEC		45     /* Executable file format error.              */
#define ENOLCK		46     /* No locks available.                        */
#define ENOLINK		47     /* Reserved.                                  */
#define ENOMEM		48     /* Not enough space.                          */
#define ENOMSG		49     /* No message of the desired type.            */
#define ENOPROTOOPT	50     /* Protocol not available.                    */
#define ENOSPC		51     /* No space left on device.                   */
#define ENOSYS		52     /* Function not supported.                    */
#define ENOTCONN	53     /* The socket is not connected.               */
#define ENOTDIR		54     /* Not a directory.                           */
#define ENOTEMPTY	55     /* Directory not empty.                       */
#define ENOTSOCK	56     /* Not a socket.                              */
#define ENOTSUP		57     /* Not supported.                             */
#define ENOTTY		58     /* Inappropriate I/O control operation.       */
#define ENXIO		59     /* No such device or address.                 */
#define EOPNOTSUPP	60     /* Operation not supported on socket.         */
#define EOVERFLOW	61     /* Value too large to be stored in data type. */
#define EPERM		62     /* Operation not permitted.                   */
#define EPIPE		63     /* Broken pipe.                               */
#define EPROTO		64     /* Protocol error.                            */
#define EPROTONOSUPPORT	65     /* Protocol not supported.                    */
#define EPROTOTYPE	66     /* Protocol wrong type for socket.            */
#define ERANGE		67     /* Result too large.                          */
#define EROFS		68     /* Read-only file system.                     */
#define ESPIPE		69     /* Invalid seek.                              */
#define ESRCH		70     /* No such process.                           */
#define ESTALE		71     /* Reserved.                                  */
#define ETIMEDOUT	72     /* Connection timed out.                      */
#define ETXTBSY		73     /* Text file busy.                            */
#define EWOULDBLOCK	EAGAIN /* Operation would block.                     */
#define EXDEV		74     /* Cross-device link.                         */

#define EEDIOT		75     /* The user is an idiot.                      */

int errno;

#endif
