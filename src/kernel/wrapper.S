/*----------------------------------------------------------------------------*\
 |	wrapper.S							      |
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

.text

.globl default_wrapper
.globl wrapper__0
.globl wrapper__1
.globl wrapper__2
.globl wrapper__3
.globl wrapper__4
.globl wrapper__5
.globl wrapper__6
.globl wrapper__7
.globl wrapper__8
.globl wrapper__9
.globl wrapper_10
.globl wrapper_11
.globl wrapper_12
.globl wrapper_13
.globl wrapper_14
.globl wrapper_16
.globl wrapper_17
.globl wrapper_18
.globl wrapper_19
.globl timer_wrapper
.globl kbd_wrapper
.globl wrapper_32
.globl wrapper_33
.globl wrapper_34
.globl wrapper_35
.globl wrapper_36
.globl wrapper_37
.globl wrapper_38
.globl wrapper_39
.globl wrapper_40
.globl wrapper_41
.globl wrapper_42
.globl wrapper_43
.globl wrapper_44
.globl wrapper_45
.globl wrapper_46
.globl wrapper_47
.globl sys_wrapper

/*----------------------------------------------------------------------------*\
 |			       default_wrapper()			      |
\*----------------------------------------------------------------------------*/
default_wrapper:
	pushal
	call	default_handler
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper__0()				      |
\*----------------------------------------------------------------------------*/
wrapper__0:
	pushal			/* Save the registers on the stack.      */
	pushl	$0		/* Push err_code onto the stack.         */
	pushl	$0		/* Push vector onto the stack.           */
	call	except		/* Call except().                        */
	addl	$8, %esp	/* Remove the arguments from the stack.  */
	popal			/* Restore the registers from the stack. */
	iret			/* Return.                               */

/*----------------------------------------------------------------------------*\
 |				  wrapper__1()				      |
\*----------------------------------------------------------------------------*/
wrapper__1:
	pushal
	pushl	$0
	pushl	$1
	call	except
	addl	$8, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper__2()				      |
\*----------------------------------------------------------------------------*/
wrapper__2:
	pushal
	pushl	$0
	pushl	$2
	call	except
	addl	$8, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper__3()				      |
\*----------------------------------------------------------------------------*/
wrapper__3:
	pushal
	pushl	$0
	pushl	$3
	call	except
	addl	$8, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper__4()				      |
\*----------------------------------------------------------------------------*/
wrapper__4:
	pushal
	pushl	$0
	pushl	$4
	call	except
	addl	$8, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper__5()				      |
\*----------------------------------------------------------------------------*/
wrapper__5:
	pushal
	pushl	$0
	pushl	$5
	call	except
	addl	$8, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper__6()				      |
\*----------------------------------------------------------------------------*/
wrapper__6:
	pushal
	pushl	$0
	pushl	$6
	call	except
	addl	$8, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper__7()				      |
\*----------------------------------------------------------------------------*/
wrapper__7:
	pushal
	pushl	$0
	pushl	$7
	call	except
	addl	$8, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper__8()				      |
\*----------------------------------------------------------------------------*/
wrapper__8:
	pushal			/* Save the registers on the stack.      */
	movl	32(%esp), %eax	/* Get the error code from the stack.    */
	pushl	%eax		/* Push err_code onto the stack.         */
	pushl	$8		/* Push vector onto the stack.           */
	call	except		/* Call except().                        */
	addl	$8, %esp	/* Remove the arguments from the stack.  */
	popal			/* Restore the registers from the stack. */
	addl	$4, %esp	/* Remove the error code from the stack. */
	iret			/* Return.                               */

/*----------------------------------------------------------------------------*\
 |				  wrapper__9()				      |
\*----------------------------------------------------------------------------*/
wrapper__9:
	pushal
	pushl	$0
	pushl	$9
	call	except
	addl	$8, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_10()				      |
\*----------------------------------------------------------------------------*/
wrapper_10:
	pushal
	pushl	$0
	pushl	$10
	call	except
	addl	$8, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_11()				      |
\*----------------------------------------------------------------------------*/
wrapper_11:
	pushal
	movl	32(%esp), %eax
	pushl	%eax
	pushl	$11
	call	except
	addl	$8, %esp
	popal
	addl	$4, %esp
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_12()				      |
\*----------------------------------------------------------------------------*/
wrapper_12:
	pushal
	movl	32(%esp), %eax
	pushl	%eax
	pushl	$12
	call	except
	addl	$8, %esp
	popal
	addl	$4, %esp
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_13()				      |
\*----------------------------------------------------------------------------*/
wrapper_13:
	pushal
	movl	32(%esp), %eax
	pushl	%eax
	pushl	$13
	call	except
	addl	$8, %esp
	popal
	addl	$4, %esp
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_14()				      |
\*----------------------------------------------------------------------------*/
wrapper_14:
	pushal
	movl	32(%esp), %eax
	pushl	%eax
	pushl	$14
	call	except
	addl	$8, %esp
	popal
	addl	$4, %esp
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_16()				      |
\*----------------------------------------------------------------------------*/
wrapper_16:
	pushal
	pushl	$0
	pushl	$16
	call	except
	addl	$8, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_17()				      |
\*----------------------------------------------------------------------------*/
wrapper_17:
	pushal
	movl	32(%esp), %eax
	pushl	%eax
	pushl	$17
	call	except
	addl	$8, %esp
	popal
	addl	$4, %esp
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_18()				      |
\*----------------------------------------------------------------------------*/
wrapper_18:
	pushal
	pushl	$0
	pushl	$18
	call	except
	addl	$8, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_19()				      |
\*----------------------------------------------------------------------------*/
wrapper_19:
	pushal
	pushl	$0
	pushl	$19
	call	except
	addl	$8, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				timer_wrapper()				      |
\*----------------------------------------------------------------------------*/
timer_wrapper:
	pushal
	call	timer_handler
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				 kbd_wrapper()				      |
\*----------------------------------------------------------------------------*/
kbd_wrapper:
	pushal
	call	kbd_handler
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_32()				      |
\*----------------------------------------------------------------------------*/
wrapper_32:
	pushal			/* Save the registers on the stack.      */
	pushl	$0		/* Push num onto the stack.              */
	call	irq_handler	/* Call irq_handler().                   */
	addl	$4, %esp	/* Remove num from the stack.            */
	popal			/* Restore the registers from the stack. */
	iret			/* Return.                               */

/*----------------------------------------------------------------------------*\
 |				  wrapper_33()				      |
\*----------------------------------------------------------------------------*/
wrapper_33:
	pushal
	pushl	$1
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_34()				      |
\*----------------------------------------------------------------------------*/
wrapper_34:
	pushal
	pushl	$2
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_35()				      |
\*----------------------------------------------------------------------------*/
wrapper_35:
	pushal
	pushl	$3
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_36()				      |
\*----------------------------------------------------------------------------*/
wrapper_36:
	pushal
	pushl	$4
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_37()				      |
\*----------------------------------------------------------------------------*/
wrapper_37:
	pushal
	pushl	$5
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_38()				      |
\*----------------------------------------------------------------------------*/
wrapper_38:
	pushal
	pushl	$6
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_39()				      |
\*----------------------------------------------------------------------------*/
wrapper_39:
	pushal
	pushl	$7
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_40()				      |
\*----------------------------------------------------------------------------*/
wrapper_40:
	pushal
	pushl	$8
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_41()				      |
\*----------------------------------------------------------------------------*/
wrapper_41:
	pushal
	pushl	$9
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_42()				      |
\*----------------------------------------------------------------------------*/
wrapper_42:
	pushal
	pushl	$10
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_43()				      |
\*----------------------------------------------------------------------------*/
wrapper_43:
	pushal
	pushl	$11
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_44()				      |
\*----------------------------------------------------------------------------*/
wrapper_44:
	pushal
	pushl	$12
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_45()				      |
\*----------------------------------------------------------------------------*/
wrapper_45:
	pushal
	pushl	$13
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_46()				      |
\*----------------------------------------------------------------------------*/
wrapper_46:
	pushal
	pushl	$14
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				  wrapper_47()				      |
\*----------------------------------------------------------------------------*/
wrapper_47:
	pushal
	pushl	$15
	call	irq_handler
	addl	$4, %esp
	popal
	iret

/*----------------------------------------------------------------------------*\
 |				 sys_wrapper()				      |
\*----------------------------------------------------------------------------*/
sys_wrapper:
	pushal
	call	do_syscall
	popal
	iret
