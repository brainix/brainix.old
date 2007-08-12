/*----------------------------------------------------------------------------*\
 |	interrupt.c							      |
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

/* Global variables: */
idt_descr_t idt[NUM_INTRS] __attribute__((aligned(8)));
unsigned char lock_count = 1;

/* Function prototypes: */
void default_handler(unsigned long instr);
void intr_set(unsigned char vector, unsigned long addr, unsigned char dpl);
void intr_init(void);
void intr_lock(void);
void intr_unlock(void);

/*----------------------------------------------------------------------------*\
 |			       default_handler()			      |
\*----------------------------------------------------------------------------*/
void default_handler(unsigned long instr)
{
	panic("default_handler", "unexpected interrupt");
}

/*----------------------------------------------------------------------------*\
 |				   intr_set()				      |
\*----------------------------------------------------------------------------*/
void intr_set(unsigned char vector, unsigned long addr, unsigned char dpl)
{
	idt[vector].offset_0 = (addr & 0x0000FFFF) >> 0;
	idt[vector].seg_sel.rpl = KERNEL_PL;
	idt[vector].seg_sel.ti = GDT;
	idt[vector].seg_sel.index = KERNEL_CODE_SEG;
	idt[vector].reserved_0 = 0;
	idt[vector].reserved_1 = 0;
	idt[vector].gate_type = INTR_GATE_32;
	idt[vector].dpl = dpl;
	idt[vector].p = true;
	idt[vector].offset_1 = (addr & 0xFFFF0000) >> 16;
}

/*----------------------------------------------------------------------------*\
 |				  intr_init()				      |
\*----------------------------------------------------------------------------*/
void intr_init(void)
{
	unsigned short j;

	for (j = 0; j < NUM_INTRS; j++)
		intr_set(j, (unsigned long) default_wrapper, KERNEL_PL);
	intr_set( 0, (unsigned long) wrapper__0, KERNEL_PL);
	intr_set( 1, (unsigned long) wrapper__1, KERNEL_PL);
	intr_set( 2, (unsigned long) wrapper__2, KERNEL_PL);
	intr_set( 3, (unsigned long) wrapper__3, KERNEL_PL);
	intr_set( 4, (unsigned long) wrapper__4, KERNEL_PL);
	intr_set( 5, (unsigned long) wrapper__5, KERNEL_PL);
	intr_set( 6, (unsigned long) wrapper__6, KERNEL_PL);
	intr_set( 7, (unsigned long) wrapper__7, KERNEL_PL);
	intr_set( 8, (unsigned long) wrapper__8, KERNEL_PL);
	intr_set( 9, (unsigned long) wrapper__9, KERNEL_PL);
	intr_set(10, (unsigned long) wrapper_10, KERNEL_PL);
	intr_set(11, (unsigned long) wrapper_11, KERNEL_PL);
	intr_set(12, (unsigned long) wrapper_12, KERNEL_PL);
	intr_set(13, (unsigned long) wrapper_13, KERNEL_PL);
	intr_set(14, (unsigned long) wrapper_14, KERNEL_PL);
	intr_set(16, (unsigned long) wrapper_16, KERNEL_PL);
	intr_set(17, (unsigned long) wrapper_17, KERNEL_PL);
	intr_set(18, (unsigned long) wrapper_18, KERNEL_PL);
	intr_set(19, (unsigned long) wrapper_19, KERNEL_PL);
	intr_set(SYS_INTR, (unsigned long) sys_wrapper, USER_PL);

	load_idtr();
}

/*----------------------------------------------------------------------------*\
 |				  intr_lock()				      |
\*----------------------------------------------------------------------------*/
void intr_lock(void)
{

/* Disable interrupts and increment the lock count.  The order is important to
 * prevent lock count from getting b0rk3d.  This is obvious, but I learned it
 * the hard way. */
	lock_count++;
	disable_intrs();
	
}

/*----------------------------------------------------------------------------*\
 |				 intr_unlock()				      |
\*----------------------------------------------------------------------------*/
void intr_unlock(void)
{

/* Decrement the lock count; if it equals zero, enable interrupts.  This routine
 * is written this way so calls to intr_lock() and intr_unlock() can be nested.
 * It looks naïve, but it works.  Trust me. */

	if (--lock_count == 0)
		enable_intrs();
}
