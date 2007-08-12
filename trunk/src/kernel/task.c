/*----------------------------------------------------------------------------*\
 |	task.c								      |
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

typedef struct task
{
	seg_descr_t *gdt_entry_ptr; /* TODO: dunno */
	tss_t tss;                  /* state of the task. mostly registers */
} task_t;

/* Global variables: */
extern seg_descr_t gdt[];
task_t task[NUM_PROCS];

/* Function prototypes: */
void task_init(void);
bool task_create(pid_t pid, unsigned long entry, bool kernel);
void task_destroy(pid_t pid);
void task_switch(pid_t pid);
void task_fork(pid_t pid_1, pid_t pid_2);
void task_exec(pid_t pid, unsigned long entry);

/*----------------------------------------------------------------------------*\
 |				  task_init()				      |
\*----------------------------------------------------------------------------*/
void task_init(void)
{

/* Initialize multi-tasking. */

	pid_t pid;
	unsigned long tss_base;
	unsigned long tss_limit = sizeof(tss_t) - 1;
	unsigned long pg_dir;
	unsigned short vague_tr;
	seg_sel_t *descr_tr = (seg_sel_t *) &vague_tr;

	/* Initialize each task in the table. */
	for (pid = 0; pid < NUM_PROCS; pid++)
	{
		task[pid].gdt_entry_ptr = &gdt[NUM_SEGS + pid];
		tss_base = (unsigned long) &task[pid].tss;
		task[pid].gdt_entry_ptr->limit_0 = 0x0000FFFF & tss_limit;
		task[pid].gdt_entry_ptr->base_0 = 0x0000FFFF & tss_base;
		task[pid].gdt_entry_ptr->base_1 = (0x00FF0000 & tss_base) >> 16;
		task[pid].gdt_entry_ptr->limit_1 = tss_limit >> 16;
		task[pid].gdt_entry_ptr->base_2 = tss_base >> 24;
		task[pid].tss.ss0.rpl = KERNEL_PL;
		task[pid].tss.ss0.ti = GDT;
		task[pid].tss.ss0.index = KERNEL_DATA_SEG;
		task[pid].tss.eflags.cf = 0;
		task[pid].tss.eflags.reserved_0 = 1;
		task[pid].tss.eflags.pf = 0;
		task[pid].tss.eflags.reserved_1 = 0;
		task[pid].tss.eflags.af = 0;
		task[pid].tss.eflags.reserved_2 = 0;
		task[pid].tss.eflags.zf = 0;
		task[pid].tss.eflags.sf = 0;
		task[pid].tss.eflags.tf = 0;
		task[pid].tss.eflags.intrf = 1;
		task[pid].tss.eflags.df = 0;
		task[pid].tss.eflags.of = 0;
		task[pid].tss.eflags.iopl = KERNEL_PL;
		task[pid].tss.eflags.nt = 0;
		task[pid].tss.eflags.reserved_3 = 0;
		task[pid].tss.eflags.rf = 0;
		task[pid].tss.eflags.vm = 0;
		task[pid].tss.eflags.ac = 0;
		task[pid].tss.eflags.vif = 0;
		task[pid].tss.eflags.vip = 0;
		task[pid].tss.eflags.id = 0;
		task[pid].tss.eflags.reserved_4 = 0;
	}

	heap_init(pg_dir = save_dir(), KERNEL); /* Create a kernel heap. */
	heap_init(pg_dir, USER);                /* Create a user heap.   */

	/* Fill in the TSS. */
	task[IDLE_PID].tss.cr3.reserved_0 = 0;
	task[IDLE_PID].tss.cr3.pwt = 0;
	task[IDLE_PID].tss.cr3.pcd = 0;
	task[IDLE_PID].tss.cr3.reserved_1 = 0;
	task[IDLE_PID].tss.cr3.sign_phys = pg_dir / PG_SIZE;

	descr_tr->rpl = USER_PL;
	descr_tr->ti = GDT;
	descr_tr->index = NUM_SEGS;

	__asm__ ("ltr %0"
		:
		:"m" (vague_tr));
}

/*----------------------------------------------------------------------------*\
 |				 task_create()				      |
\*----------------------------------------------------------------------------*/
bool task_create(pid_t pid, unsigned long entry, bool kernel)
{

/* Create a task. */

	unsigned long pg_dir = pg_space_create();

	/* Create a virtual address space, kernel stack, user heap, and user
	 * stack. */
	if (!pg_dir                  || !stack_init(pg_dir, KERNEL) ||
	    !heap_init(pg_dir, USER) || !stack_init(pg_dir, USER))
	{
		/* The virtual address space, kernel stack, user heap, or user
		 * stack couldn't be created. */
		if (pg_dir)
			pg_space_destroy(pg_dir);
		return false;
	}

	/* Fill in the TSS. */
	task[pid].tss.esp0 = pg_addr(KERNEL, STACK, TOP);
	task[pid].tss.cr3.reserved_0 = 0;
	task[pid].tss.cr3.pwt = 0;
	task[pid].tss.cr3.pcd = 0;
	task[pid].tss.cr3.reserved_1 = 0;
	task[pid].tss.cr3.sign_phys = pg_dir / PG_SIZE;
	task[pid].tss.eip = entry;
	task[pid].tss.esp = pg_addr(USER, STACK, TOP);
	task[pid].tss.es.rpl = kernel ? KERNEL_PL : USER_PL;
	task[pid].tss.cs.rpl = kernel ? KERNEL_PL : USER_PL;
	task[pid].tss.ss.rpl = kernel ? KERNEL_PL : USER_PL;
	task[pid].tss.ds.rpl = kernel ? KERNEL_PL : USER_PL;
	task[pid].tss.fs.rpl = kernel ? KERNEL_PL : USER_PL;
	task[pid].tss.gs.rpl = kernel ? KERNEL_PL : USER_PL;
	task[pid].tss.es.ti = GDT;
	task[pid].tss.cs.ti = GDT;
	task[pid].tss.ss.ti = GDT;
	task[pid].tss.ds.ti = GDT;
	task[pid].tss.fs.ti = GDT;
	task[pid].tss.gs.ti = GDT;
	task[pid].tss.es.index = kernel ? KERNEL_DATA_SEG : USER_DATA_SEG;
	task[pid].tss.cs.index = kernel ? KERNEL_CODE_SEG : USER_CODE_SEG;
	task[pid].tss.ss.index = kernel ? KERNEL_DATA_SEG : USER_DATA_SEG;
	task[pid].tss.ds.index = kernel ? KERNEL_DATA_SEG : USER_DATA_SEG;
	task[pid].tss.fs.index = kernel ? KERNEL_DATA_SEG : USER_DATA_SEG;
	task[pid].tss.gs.index = kernel ? KERNEL_DATA_SEG : USER_DATA_SEG;

	return true;
}

/*----------------------------------------------------------------------------*\
 |				 task_destroy()				      |
\*----------------------------------------------------------------------------*/
void task_destroy(pid_t pid)
{

/* Destroy a task. */

	unsigned long pg_dir = task[pid].tss.cr3.sign_phys * PG_SIZE;

	/* Destroy the task's virtual address space. */
	pg_space_destroy(pg_dir);
}

/*----------------------------------------------------------------------------*\
 |				 task_switch()				      |
\*----------------------------------------------------------------------------*/
void task_switch(pid_t pid)
{
	log_addr_t far;

	task[pid].gdt_entry_ptr->type = SYSTEM_GATE_TSS_32_AVAILABLE;

	far.offset = 0;
	far.seg_sel.rpl = USER_PL;
	far.seg_sel.ti = GDT;
	far.seg_sel.index = NUM_SEGS + pid;

	__asm__ ("ljmp *%0"
		:
		:"m" (far));
}

/*----------------------------------------------------------------------------*\
 |				  task_fork()				      |
\*----------------------------------------------------------------------------*/
void task_fork(pid_t pid_1, pid_t pid_2)
{

/* Perform the task manager's part of the fork() system call. */

	unsigned long pg_dir_1 = task[pid_1].tss.cr3.sign_phys * PG_SIZE;
	unsigned long pg_dir_2 = task[pid_2].tss.cr3.sign_phys * PG_SIZE;
	pg_fork(pg_dir_1, pg_dir_2);
	task[pid_2].tss = task[pid_1].tss;
	task[pid_2].tss.cr3.sign_phys = pg_dir_2 / PG_SIZE;
}

/*----------------------------------------------------------------------------*\
 |				  task_exec()				      |
\*----------------------------------------------------------------------------*/
void task_exec(pid_t pid, unsigned long entry)
{

/* Perform the task manager's part of the exec() system call. */

	unsigned long pg_dir_1 = task[FS_PID].tss.cr3.sign_phys * PG_SIZE;
	unsigned long pg_dir_2 = task[pid].tss.cr3.sign_phys * PG_SIZE;
	pg_exec(pg_dir_1, pg_dir_2);
	task[pid].tss.eip = entry;
	task[pid].tss.esp = pg_addr(USER, STACK, TOP);
}
