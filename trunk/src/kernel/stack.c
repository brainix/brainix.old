/*----------------------------------------------------------------------------*\
 |	stack.c								      |
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

/* Function prototypes: */
bool stack_pg_fault(bool kernel, unsigned long fault_addr);
bool stack_init(unsigned long pg_dir, bool kernel);

/*----------------------------------------------------------------------------*\
 |				stack_pg_fault()			      |
\*----------------------------------------------------------------------------*/
bool stack_pg_fault(bool kernel, unsigned long fault_addr)
{
	unsigned long pg_dir = save_dir();
	unsigned long stack_bottom = pg_next_addr(pg_dir, kernel, STACK);
	unsigned long next_stack_bottom = stack_bottom - PG_SIZE;

	if (fault_addr > stack_bottom || fault_addr <= next_stack_bottom)
		return false;
	return pg_alloc(pg_dir, kernel, STACK) != 0;
}

/*----------------------------------------------------------------------------*\
 |				  stack_init()				      |
\*----------------------------------------------------------------------------*/
bool stack_init(unsigned long pg_dir, bool kernel)
{
	return pg_alloc(pg_dir, kernel, STACK) != 0;
}
