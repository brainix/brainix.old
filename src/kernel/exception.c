/*----------------------------------------------------------------------------*\
 |	exception.c							      |
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



/*----------------------------------------------------------------------------*\
 | The following macros are used to examine the page-fault error code:	      |
\*----------------------------------------------------------------------------*/

/* 0 if the fault was caused by a non-present page;
 * 1 if the fault was caused by a page-level protection violation: */
#define PG_FAULT_P(err_code) \
	((err_code & 0x1) == 0x1)

/* 0 if the access causing the fault was a read;
 * 1 if the access causing the fault was a write: */
#define PG_FAULT_RW(err_code) \
	((err_code & 0x2) == 0x2)

/* 0 if the access causing the fault originated in supervisor (kernel) mode;
 * 1 if the access causing the fault originated in user mode: */
#define PG_FAULT_US(err_code) \
	((err_code & 0x4) == 0x4)

/* 0 if the fault was not caused by reserved bit violation;
 * 1 if the fault was caused by reserved bits set to 1 in a page directory: */
#define PG_FAULT_RSVD(err_code) \
	((err_code & 0x8) == 0x8)



/*----------------------------------------------------------------------------*\
 | The following macros are used to examine the page-fault faulting address:  |
\*----------------------------------------------------------------------------*/

/* True iff the faulting address lies in kernel heap space: */
#define KERNEL_HEAP_SPACE(addr) \
	(addr >= pg_addr(KERNEL, HEAP, BOTTOM) && \
	 addr  < pg_addr(KERNEL, HEAP, TOP))

/* True iff the faulting address lies in kernel stack space: */
#define KERNEL_STACK_SPACE(addr) \
	(addr >= pg_addr(KERNEL, STACK, BOTTOM) && \
	 addr  < pg_addr(KERNEL, STACK, TOP))

/* True iff the faulting address lies in user heap space: */
#define USER_HEAP_SPACE(addr) \
	(addr >= pg_addr(USER, HEAP, BOTTOM) && \
	 addr  < pg_addr(USER, HEAP, TOP))

/* True iff the faulting address lies in user stack space: */
#define USER_STACK_SPACE(addr) \
	(addr >= pg_addr(USER, STACK, BOTTOM) && \
	 addr  < pg_addr(USER, STACK, TOP))

/* True iff the faulting address lies in kernel space: */
#define KERNEL_SPACE(addr) \
	(KERNEL_HEAP_SPACE(addr) || KERNEL_STACK_SPACE(addr))

/* True iff the faulting address lies in user space: */
#define USER_SPACE(addr) \
	(USER_HEAP_SPACE(addr) || USER_STACK_SPACE(addr))

/* True iff the faulting address lies in heap space: */
#define HEAP_SPACE(addr) \
	(KERNEL_HEAP_SPACE(addr) || USER_HEAP_SPACE(addr))

/* True iff the faulting address lies in stack space: */
#define STACK_SPACE(addr) \
	(KERNEL_STACK_SPACE(addr) || USER_STACK_SPACE(addr))



/* Function prototypes: */
bool pg_fault(unsigned long err_code);
void except(unsigned char vector, unsigned long err_code, stack_t s);



/*----------------------------------------------------------------------------*\
 |				   pg_fault()				      |
\*----------------------------------------------------------------------------*/
bool pg_fault(unsigned long err_code)
{

/* A page-fault exception has occurred.  Try to handle it.  Return true on
 * success or false on failure. */

	/* Save the faulting address. */
	unsigned long addr = get_fault_addr();

	/* Perform a few sanity checks. */
	if (PG_FAULT_P(err_code) || PG_FAULT_RSVD(err_code))
		/* The fault was caused by a page-level protection violation or
		 * reserved bit violation. */
		panic("pg_fault", "protection or reserved bit");
	if (KERNEL_SPACE(addr) && PG_FAULT_US(err_code))
		/* The faulting address lies in kernel space but the access
		 * causing the fault originated in user mode. */
		panic("pg_fault", "kernel space, but user mode");
	if (USER_SPACE(addr) && !PG_FAULT_US(err_code))
		/* The faulting address lies in user space but the access
		 * causing the fault originated in kernel mode. */
		panic("pg_fault", "user space, but kernel mode");
	if (STACK_SPACE(addr) && !PG_FAULT_RW(err_code))
		/* The faulting address lies in stack space but the access
		 * causing the fault was a read. */
		panic("pg_fault", "stack space, but read");

	/* Try to resolve the page-fault. */
	if (KERNEL_HEAP_SPACE(addr))
	{
		if (heap_pg_fault(KERNEL, addr))
			return true;
		panic("pg_fault", "in kernel heap space");
	}
	if (KERNEL_STACK_SPACE(addr))
	{
		if (stack_pg_fault(KERNEL, addr))
			return true;
		panic("pg_fault", "in kernel stack space");
	}
	if (USER_HEAP_SPACE(addr))
	{
		if (heap_pg_fault(USER, addr))
			return true;
		panic("pg_fault", "in user heap space");
	}
	if (USER_STACK_SPACE(addr))
	{
		if (stack_pg_fault(USER, addr))
			return true;
		panic("pg_fault", "in user stack space");
	}

	/* Prevent the compiler from bitching and moaning. */
	panic("pg_fault", "in unknown space");
	return false;
}



char *panic_message[] =
{
/*  0 */	"divide error exception",
/*  1 */	"debug exception",
/*  2 */	"NMI interrupt",
/*  3 */	"breakpoint exception",
/*  4 */	"overflow exception",
/*  5 */	"BOUND range exceeded exception",
/*  6 */	"invalid opcode exception",
/*  7 */	"device not available exception",
/*  8 */	"double fault exception",
/*  9 */	"coprocessor segment overrun",
/* 10 */	"invalid TSS exception",
/* 11 */	"segment not present",
/* 12 */	"stack fault exception",
/* 13 */	"general protection exception",
/* 14 */	"page-fault exception",
/*    */	"",
/* 16 */	"x87 FPU floating-point error",
/* 17 */	"alignment check exception",
/* 18 */	"machine-check exception",
/* 19 */	"SIMD floating-point exception"
};



/*----------------------------------------------------------------------------*\
 |				    except()				      |
\*----------------------------------------------------------------------------*/
void except(unsigned char vector, unsigned long err_code, stack_t s)
{

/* An exception has occurred.  Try to handle it. */

	bool handled = false;

	/* Try to handle the exception. */
	intr_lock();
	switch (vector)
	{
		case  0:                               break;
		case  1:                               break;
		case  2:                               break;
		case  3:                               break;
		case  4:                               break;
		case  5:                               break;
		case  6:                               break;
		case  7:                               break;
		case  8:                               break;
		case  9:                               break;
		case 10:                               break;
		case 11:                               break;
		case 12:                               break;
		case 13:                               break;
		case 14: handled = pg_fault(err_code); break;
		case 15:                               break;
		case 16:                               break;
		case 17:                               break;
		case 18:                               break;
		case 19:                               break;
	}
	intr_unlock();

	if (!handled)
		/* The exception could not be handled. */
		panic("except", panic_message[vector]);
}
