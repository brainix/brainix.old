/*----------------------------------------------------------------------------*\
 |	segment.c							      |
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

#define BRAINIX
#include <kernel/kernel.h>

/* Global variable: */
seg_descr_t gdt[NUM_SEGS + NUM_PROCS] __attribute__((aligned(8)));

/* Function prototype: */
void seg_init(void);

/*----------------------------------------------------------------------------*\
 |				   seg_init()				      |
\*----------------------------------------------------------------------------*/
void seg_init(void)
{

/*
 | Initialize segmentation.  Brainix uses the basic flat model:
 |
 | The simplest memory model for a system is the basic "flat model," in which
 | the operating system and application programs have access to a continuous,
 | unsegmented address space.  To the greatest extent possible, this basic flat
 | model hides the segmentation mechanism of the architecture from both the
 | system designer and the application programmer.
 */

	unsigned char j;

	gdt[UNUSED_SEG].limit_0 =
	gdt[UNUSED_SEG].base_0 =
	gdt[UNUSED_SEG].base_1 =
	gdt[UNUSED_SEG].type =
	gdt[UNUSED_SEG].s =
	gdt[UNUSED_SEG].dpl =
	gdt[UNUSED_SEG].p =
	gdt[UNUSED_SEG].limit_1 =
	gdt[UNUSED_SEG].avl =
	gdt[UNUSED_SEG].reserved =
	gdt[UNUSED_SEG].db =
	gdt[UNUSED_SEG].g =
	gdt[UNUSED_SEG].base_2 = 0;

	for (j = KERNEL_CODE_SEG; j <= USER_DATA_SEG; j++)
	{
		gdt[j].limit_0 = 0xFFFF;
		gdt[j].base_0 = 0;
		gdt[j].base_1 = 0;
		gdt[j].type = CODE_SEG(j) ? CODE_EXEC_READ : DATA_READ_WRITE;
		gdt[j].s = CODE_OR_DATA;
		gdt[j].dpl = KERNEL_SEG(j) ? KERNEL_PL : USER_PL;
		gdt[j].p = true;
		gdt[j].limit_1 = 0xF;
		gdt[j].avl = 0;
		gdt[j].reserved = 0;
		gdt[j].db = DEFAULT_OPERATION_32;
		gdt[j].g = GRANULARITY_4K;
		gdt[j].base_2 = 0;
	}

	for (j = NUM_SEGS; j < NUM_SEGS + NUM_PROCS; j++)
	{
		gdt[j].type = SYSTEM_GATE_TSS_32_AVAILABLE;
		gdt[j].s = SYSTEM;
		gdt[j].dpl = USER_PL;
		gdt[j].p = true;
		gdt[j].avl = 0;
		gdt[j].reserved = 0;
		gdt[j].db = DEFAULT_OPERATION_16;
		gdt[j].g = GRANULARITY_1B;
	}

	load_gdtr();
}
