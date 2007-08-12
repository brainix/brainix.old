/*----------------------------------------------------------------------------*\
 |	time.c								      |
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

#include <kernel/syscall.h>
#include <time.h>

/*----------------------------------------------------------------------------*\
 |				     time()				      |
\*----------------------------------------------------------------------------*/
time_t time(time_t *tloc)
{

/* Get time. */

	return syscall(SYS_TIME, (unsigned long) tloc, 0, 0);
}
