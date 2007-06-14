/*----------------------------------------------------------------------------*\
 |	main.c								      |
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
void main(unsigned long magic, unsigned long multiboot_info_addr);
void kernel(void);
void shutdown(msg_t *msg);
void print_init(bool init, char *s);
void print_done(void);

/*----------------------------------------------------------------------------*\
 |				     main()				      |
\*----------------------------------------------------------------------------*/
void main(unsigned long magic, unsigned long multiboot_info_addr)
{
	multiboot_info_t *multiboot_info = (multiboot_info_t *)
		multiboot_info_addr;

	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	clear();
	set_attr(SOLID, SEVERE_BG, SEVERE_FG);
	print_version();
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf("\n");

	print_init(INIT, "hardware");
	seg_init();
	intr_init();
	print_done();

	print_init(INIT, "kernel");
	paging_init(multiboot_info->mem_upper);
	proc_init();
	msg_init();
	irq_init();
	proc_create((unsigned long) kernel, KERNEL);
	print_done();

	print_init(INIT, "timer");
	proc_create((unsigned long) timer_main, KERNEL);
	print_done();

	print_init(INIT, "file system");
	proc_create((unsigned long) fs_main, KERNEL);
	print_done();

	print_init(INIT, "drivers");
	proc_create((unsigned long) mem_main, KERNEL);
	proc_create((unsigned long) fdc_main, KERNEL);
	proc_create((unsigned long) tty_main, KERNEL);
	print_done();

	printf("\n");
	proc_sched();
	while (true)
		halt();
}

/*----------------------------------------------------------------------------*\
 |				    kernel()				      |
\*----------------------------------------------------------------------------*/
void kernel(void)
{
	msg_t *msg;

	while (true)
		switch ((msg = msg_receive(ANYONE))->op)
		{
			case SYS_FORK:
				proc_fork(msg);
				msg_reply(msg);
				continue;
			case SYS_EXECVE:
				proc_exec(msg);
				msg_free(msg);
				continue;
			case SYS_EXIT:
				proc_exit(msg);
				msg_free(msg);
				continue;
			case REGISTER:
				irq_register(msg);
				msg_reply(msg);
				continue;
			case SHUTDOWN:
				shutdown(msg);
				msg_free(msg);
				continue;
			default:
				panic("kernel", "unexpected message");
				msg_free(msg);
				continue;
		}
}

/*----------------------------------------------------------------------------*\
 |				   shutdown()				      |
\*----------------------------------------------------------------------------*/
void shutdown(msg_t *msg)
{
	msg_t *msg;
	mid_t mid;

	printf("\n");

	print_init(DEINIT, "user processes");
	print_done();

	print_init(DEINIT, "file system");
	print_done();

	print_init(DEINIT, "timer");
	mid = (msg = msg_alloc(TMR_PID, SHUTDOWN))->mid;
	msg_send(msg);
	msg_free(msg_receive(mid));
	print_done();

	print_init(DEINIT, "hardware");
	if (msg->args.brnx_shutdown.reboot)
		kbd_reboot();
	print_done();

	disable_intrs();
	halt();
}

/*----------------------------------------------------------------------------*\
 |				  print_init()				      |
\*----------------------------------------------------------------------------*/
void print_init(bool init, char *s)
{
	unsigned char j;

	set_attr(SOLID, UNIMPORTANT_BG, UNIMPORTANT_FG);
	if (!init)
		printf("de");
	printf("init");
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf(" %s ", s);
	for (j = strlen(s); j < strlen("file system"); j++)
		printf(" ");
	set_attr(SOLID, UNIMPORTANT_BG, UNIMPORTANT_FG);
	printf("...");
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
	printf(" ");
	print_emoticon(THINK);
}

/*----------------------------------------------------------------------------*\
 |				  print_done()				      |
\*----------------------------------------------------------------------------*/
void print_done(void)
{
	print_emoticon(ERASE);
	print_emoticon(SMILE);
	printf("\n");
}
