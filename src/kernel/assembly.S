/*----------------------------------------------------------------------------*\
 |	assembly.S							      |
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

/*
 | Programming in assembly is like doing construction work with a toothpick for
 | 	a tool. - source unknown
 |
 | Programming in C is like picking your teeth with a jack-hammer. - R. Shah
 */

/*
 | This file contains various assembly language routines needed by the kernel.
 | These routines play nicely with the C compiler; they can be called from/like
 | normal C functions.
 |
 | In order to play nicely with the C compiler, these routines:
 |	· preserve EBX, ESI, EDI, EBP, DS, ES, SS
 |	· get arguments from the stack
 |	· return values in EAX
 */

#include <config.h>
#include <const.h>

/* Flags in control register 0: */
#define CR0_PE	(1 <<  0)	/* Protection enable.   */
#define CR0_MP	(1 <<  1)	/* Monitor coprocessor. */
#define CR0_EM	(1 <<  2)	/* Emulation.           */
#define CR0_TS	(1 <<  3)	/* Task switched.       */
#define CR0_ET	(1 <<  4)	/* Extension type.      */
#define CR0_NE	(1 <<  5)	/* Numeric error.       */
#define CR0_WP	(1 << 16)	/* Write protect.       */
#define CR0_AM	(1 << 18)	/* Alignment mask.      */
#define CR0_NW	(1 << 29)	/* Not write-through.   */
#define CR0_CD	(1 << 30)	/* Cache disable.       */
#define CR0_PG	(1 << 31)	/* Paging.              */

.text

gdtr:
	.word	(8 * (NUM_SEGS + NUM_PROCS) - 1)
	.long	gdt
idtr:
	.word	(8 * NUM_INTRS - 1)
	.long	idt

.globl in_byte			/* Input byte from I/O port.                 */
.globl out_byte			/* Output byte to I/O port.                  */
.globl io_delay			/* Delay for slow devices.                   */
.globl load_gdtr		/* Load global descriptor table register.    */
.globl load_idtr		/* Load interrupt descriptor table register. */
.globl disable_intrs		/* Disable interrupts.                       */
.globl enable_intrs		/* Enable interrupts.                        */
.globl save_dir			/* Save page directory.                      */
.globl load_dir			/* Load page directory.                      */
.globl invalidate_tlbs		/* Invalidate translation lookaside buffers. */
.globl disable_paging		/* Disable paging.                           */
.globl enable_paging		/* Enable paging.                            */
.globl get_fault_addr		/* Get page fault address.                   */
.globl halt			/* Halt processor.                           */

/*----------------------------------------------------------------------------*\
 |				   in_byte()				      |
\*----------------------------------------------------------------------------*/
in_byte:
	movl	4(%esp), %edx	/* Get I/O port from stack.  */
	movl	$0, %eax	/* Initialize return value.  */
	inb	%dx, %al	/* Input byte from I/O port. */
	ret			/* Return.                   */

/*----------------------------------------------------------------------------*\
 |				   out_byte()				      |
\*----------------------------------------------------------------------------*/
out_byte:
	movl	4(%esp), %eax	/* Get byte from stack.     */
	movl	8(%esp), %edx	/* Get I/O port from stack. */
	outb	%al, %dx	/* Output byte to I/O port. */
	ret			/* Return.                  */

/*----------------------------------------------------------------------------*\
 |				   io_delay()				      |
\*----------------------------------------------------------------------------*/
io_delay:
	movl	4(%esp), %ecx	/* Get count from stack.    */
label:	outb	%al, $0x80	/* Output byte to I/O port. */
	loopl	label		/* Loop count times.        */
	ret			/* Return.                  */

/*----------------------------------------------------------------------------*\
 |				  load_gdtr()				      |
\*----------------------------------------------------------------------------*/
load_gdtr:
	lgdt	gdtr		/* Load global descriptor table register. */
	ret			/* Return.                                */

/*----------------------------------------------------------------------------*\
 |				  load_idtr()				      |
\*----------------------------------------------------------------------------*/
load_idtr:
	lidt	idtr		/* Load interrupt descriptor table register. */
	ret			/* Return.                                   */

/*----------------------------------------------------------------------------*\
 |				disable_intrs()				      |
\*----------------------------------------------------------------------------*/
disable_intrs:
	cli			/* Disable interrupts. */
	ret			/* Return.             */

/*----------------------------------------------------------------------------*\
 |				 enable_intrs()				      |
\*----------------------------------------------------------------------------*/
enable_intrs:
	sti			/* Enable interrupts. */
	ret			/* Return.            */

/*----------------------------------------------------------------------------*\
 |				   save_dir()				      |
\*----------------------------------------------------------------------------*/
save_dir:
	movl	%cr3, %eax	/* Save page directory. */
	ret			/* Return.              */

/*----------------------------------------------------------------------------*\
 |				   load_dir()				      |
\*----------------------------------------------------------------------------*/
load_dir:
	movl	4(%esp), %eax	/* Get page directory from stack. */
	movl	%eax, %cr3	/* Load page directory.           */
	ret			/* Return.                        */

/*----------------------------------------------------------------------------*\
 |			       invalidate_tlbs()			      |
\*----------------------------------------------------------------------------*/
invalidate_tlbs:
	movl	%cr3, %eax	/* Save page directory. */
	movl	%eax, %cr3	/* Load page directory. */
	ret			/* Return.              */

/*----------------------------------------------------------------------------*\
 |				disable_paging()			      |
\*----------------------------------------------------------------------------*/
disable_paging:
	movl	%cr0, %eax	/* Save control register 0. */
	andl	$~CR0_PG, %eax	/* Clear paging bit.        */
	movl	%eax, %cr0	/* Load control register 0. */
	ret			/* Return.                  */

/*----------------------------------------------------------------------------*\
 |				enable_paging()				      |
\*----------------------------------------------------------------------------*/
enable_paging:
	movl	%cr0, %eax	/* Save control register 0. */
	orl	$CR0_PG, %eax	/* Set paging bit.          */
	movl	%eax, %cr0	/* Load control register 0. */
	ret			/* Return.                  */

/*----------------------------------------------------------------------------*\
 |				get_fault_addr()			      |
\*----------------------------------------------------------------------------*/
get_fault_addr:
	movl	%cr2, %eax	/* Save page fault address. */
	ret			/* Return.                  */

/*----------------------------------------------------------------------------*\
 |				     halt()				      |
\*----------------------------------------------------------------------------*/
halt:
	hlt			/* Halt processor. */
	ret			/* Return.         */
