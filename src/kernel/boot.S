/*----------------------------------------------------------------------------*\
 |	boot.S								      |
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

#define ASM
#include <kernel/multiboot.h>

.text

.globl _start

_start:
	jmp	multiboot_entry

.align 4
multiboot_header:
	.long	MULTIBOOT_HEADER_MAGIC
	.long	MULTIBOOT_HEADER_FLAGS
	.long	-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
	.long	multiboot_header
	.long	_start
	.long	_edata
	.long	_end
	.long	multiboot_entry

multiboot_entry:
	movl	$(stack + STACK_SIZE), %esp
	pushl	$0
	popf
	pushl	%ebx
	pushl	%eax
	call	main

.comm stack, STACK_SIZE
