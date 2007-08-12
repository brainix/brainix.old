/*----------------------------------------------------------------------------*\
 |	panic.c								      |
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

/* Function prototype: */
void panic(char *func, char *msg);

/*----------------------------------------------------------------------------*\
 |				    panic()				      |
\*----------------------------------------------------------------------------*/
void panic(char *func, char *msg)
{

/* Houston, we have a problem... */

	disable_intrs();

	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf("\n");

	set_attr(SOLID, SEVERE_BG, SEVERE_FG);
	printf("kernel panic!");
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf("\n  ");

	set_attr(SOLID, IMPORTANT_BG, IMPORTANT_FG);
	printf("process");
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf("  ");
	set_attr(SOLID, UNIMPORTANT_BG, UNIMPORTANT_FG);
	printf(":");
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf(" ");
	print_pid(do_getpid());
	printf("\n  ");

	set_attr(SOLID, IMPORTANT_BG, IMPORTANT_FG);
	printf("function");
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf(" ");
	set_attr(SOLID, UNIMPORTANT_BG, UNIMPORTANT_FG);
	printf(":");
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf(" %s()\n  ", func);

	set_attr(SOLID, IMPORTANT_BG, IMPORTANT_FG);
	printf("message");
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf("  ");
	set_attr(SOLID, UNIMPORTANT_BG, UNIMPORTANT_FG);
	printf(":");
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf(" %s\n", msg);

	set_attr(SOLID, SEVERE_BG, SEVERE_FG);
	printf("system halted");

	halt();
}
