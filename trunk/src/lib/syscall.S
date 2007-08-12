/*----------------------------------------------------------------------------*\
 |	syscall.S							      |
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

/*
 | unsigned long syscall(unsigned char num,
 |			 unsigned long arg1,
 |			 unsigned long arg2,
 |			 unsigned long arg3);
 */

.text

.globl syscall

/*----------------------------------------------------------------------------*\
 |				   syscall()				      |
\*----------------------------------------------------------------------------*/
syscall:
	pushl	%ebx		/* Save EBX on the stack.          */
	movl	 8(%esp), %eax	/* Pass the syscall number in EAX. */
	movl	12(%esp), %ecx	/* Pass the 1st argument in ECX.   */
	movl	16(%esp), %edx	/* Pass the 2nd argument in EDX.   */
	movl	20(%esp), %ebx	/* Pass the 3rd argument in EBX.   */
	int	$128		/* Make the magic happen!          */
	popl	%ebx		/* Restore EBX from the stack.     */
	ret			/* Return.                         */
