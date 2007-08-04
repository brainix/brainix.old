/*----------------------------------------------------------------------------*\
 |	paging.c							      |
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

#define KERNEL_BASE		(1 * MB)
#define KERNEL_SIZE		(1 * MB)
#define TBL_MAPPING		(KERNEL_BASE + KERNEL_SIZE)
#define TMP_DIR_MAPPING		(TBL_MAPPING + PG_SIZE)
#define TMP_TBL_MAPPING		(TMP_DIR_MAPPING + PG_SIZE)
#define PG_STACK_BASE		(TMP_TBL_MAPPING + PG_SIZE)

/*
 * Base of Physical Memory.
 *
 * Every process has its virtual memory identity
 * mapped below KERNEL_HEAP_BOTTOM.  any address
 * above this uses the paging system to map its
 * virtual addresses to its physical addresses.
 *
 * Note that even the kernel process uses virtual 
 * memory, and so addresses pointing to the kernel
 * heap are also mapped through the pagging system.
 *
 * Note that two pages are residing at the base
 * of the kernel heap space.  These pages are not
 * identity mapped to this location, so
 * they don't interfere with any kernel heap slabs.
 *
 *
 * #~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~~# 
 * #                        #
 * #                  /|\   #
 * #      Kernel Heap  |    #
 * #                   |    #
 * #                        #
 * # sys page table entries #
 * #  (directory + tables)  #
 * #------------------------# KERNEL_HEAP_BOTTOM  ~2MB + 12*num_pgs 
 * #                        #
 * #   page stack entries   #
 * #                        #
 * #------------------------# PG_STACK_BASE 
 * #------------------------# 2MB TBL_MAPPING
 * #                        #
 * #         kernel         #
 * #                        #
 * #------------------------# 1MB KERNEL_BASE
 * #                        #
 * #         unused         #
 * #                        #
 * ########################## 0
 */

#define KERNEL_HEAP_BOTTOM	(sys_pgs * PG_SIZE)
#define KERNEL_HEAP_TOP		(1 * GB)
#define KERNEL_STACK_BOTTOM	KERNEL_HEAP_TOP
#define KERNEL_STACK_TOP	(2 * GB)
#define USER_HEAP_BOTTOM	KERNEL_STACK_TOP
#define USER_HEAP_TOP		(3 * GB)
#define USER_STACK_BOTTOM	USER_HEAP_TOP
#define USER_STACK_TOP		(4 * GB)

#define PG_DIR			true
#define PG_TBL			false

/*
 * linked list node to hold reference to one page's
 * physical address 'phys'.  pg_stack's are allocated
 * at PG_STACK_BASE and pushed/popped on the 'free_pg'
 * and 'used_pg' stacks depending on whether the page
 * of memory it references is being used.
 */
typedef struct pg_stack
{
	unsigned long phys;
	struct pg_stack *prev;
	struct pg_stack *next;
} pg_stack_t;

/* Global variables: */
unsigned long num_pgs;	/* total number of pages available */
unsigned long sys_pgs;	/* number of pages reserved for kernel usage */
pg_stack_t *free_pg;	/* stack referencing unused pages */
pg_stack_t *used_pg;	/* ... pages in use */

/* Function prototypes: */
void pg_stack_init(void);
unsigned long pg_pop(void);
void pg_push(unsigned long phys);
void pg_dir_new(pde_t *dir);
void pg_tbl_new(pte_t *tbl);
unsigned long pg_space_create_init(void);
void paging_init(unsigned long mem_upper);
unsigned long pg_dir_tbl_map(unsigned long phys, bool dir);
pde_t *pg_dir_map(unsigned long pg_dir);
pte_t *pg_tbl_map(unsigned long pg_tbl);
void pg_map(unsigned long pg_dir, unsigned long phys, unsigned long lin,
	unsigned char us);
void pg_unmap(unsigned long pg_dir, unsigned long lin);
unsigned long pg_space_create(void);
void pg_space_destroy(unsigned long pg_dir);
unsigned long pg_addr(bool kernel, bool heap, bool bottom);
bool pg_mapped(unsigned long pg_dir, unsigned long lin);
unsigned long pg_next_addr(unsigned long pg_dir, bool kernel, bool heap);
unsigned long pg_alloc(unsigned long pg_dir, bool kernel, bool heap);
void pg_free(unsigned long pg_dir, unsigned long lin);
void pg_copy(unsigned long phys_1, unsigned long phys_2);
void pg_fork(unsigned long pg_dir_1, unsigned long pg_dir_2);
void pg_exec(unsigned long pg_dir_1, unsigned long pg_dir_2);

/*----------------------------------------------------------------------------*\
 |				pg_stack_init()				      |
\*----------------------------------------------------------------------------*/
void pg_stack_init(void)
{

/* Initialize the page stack. */

	pg_stack_t *tmp = (pg_stack_t *) PG_STACK_BASE;
	unsigned long j;

	for (j = 0; j < num_pgs; j++)
	{
		tmp[j].phys = j * PG_SIZE;
		tmp[j].prev = &tmp[j - 1];
		tmp[j].next = &tmp[j + 1];
	}
	tmp[0].prev = tmp[num_pgs - 1].next = NULL;

	free_pg = tmp;
	used_pg = NULL;
}

/*----------------------------------------------------------------------------*\
 |				    pg_pop()				      |
\*----------------------------------------------------------------------------*/
unsigned long pg_pop(void)
{

/* Pop a page off of the stack. */

	pg_stack_t *tmp = free_pg;

	if (tmp == NULL)
		panic("pg_pop", "out of free pages");

	if ((free_pg = free_pg->next) != NULL)
		free_pg->prev = NULL;
	/*
	 * tmp->next = used_pg;
	 * used_pg->prev = tmp;
	 * used_pg = tmp;
	 * return tmp->phys;
	 */
	return (used_pg = (tmp->next = used_pg)->prev = tmp)->phys;
}

/*----------------------------------------------------------------------------*\
 |				   pg_push()				      |
\*----------------------------------------------------------------------------*/
void pg_push(unsigned long phys)
{

/* Push a page onto the stack. */

	pg_stack_t *tmp;

	for (tmp = used_pg; tmp->phys != phys; tmp = tmp->next)
		;
	if (tmp->prev == NULL)
	{
		if ((used_pg = used_pg->next) != NULL)
			used_pg->prev = NULL;
	}
	else
		tmp->prev->next = tmp->next;
	if (tmp->next != NULL)
		tmp->next->prev = tmp->prev;

	(free_pg = (tmp->next = free_pg)->prev = tmp)->prev = NULL;
}

/*----------------------------------------------------------------------------*\
 |				  pg_dir_new()				      |
\*----------------------------------------------------------------------------*/
void pg_dir_new(pde_t *dir)
{

/* Initialize a page directory. */

	unsigned short j;

	for (j = 0; j < 1024; j++)
	{
		dir[j].p = 0;
		dir[j].rw = 1;
		dir[j].us = 1;
		dir[j].pwt = 0;
		dir[j].pcd = 0;
		dir[j].a = 0;
		dir[j].reserved = 0;
		dir[j].ps = 0;
		dir[j].g = 0;
		dir[j].avl = 0;
		dir[j].sign_phys = 0;
	}
}

/*----------------------------------------------------------------------------*\
 |				  pg_tbl_new()				      |
\*----------------------------------------------------------------------------*/
void pg_tbl_new(pte_t *tbl)
{

/* Initialize a page table. */

	unsigned short j;

	for (j = 0; j < 1024; j++)
	{
		tbl[j].p = 0;
		tbl[j].rw = 1;
		tbl[j].us = 1;
		tbl[j].pwt = 0;
		tbl[j].pcd = 0;
		tbl[j].a = 0;
		tbl[j].d = 0;
		tbl[j].pat = 0;
		tbl[j].g = 0;
		tbl[j].avl = 0;
		tbl[j].sign_phys = 0;
	}
}

/*----------------------------------------------------------------------------*\
 |			     pg_space_create_init()			      |
\*----------------------------------------------------------------------------*/
unsigned long pg_space_create_init(void)
{
 	/*
 	 * paging_init has already popped off enough pages for the system,
 	 * so this should return the first page of physical memory where
 	 * the kernel heap would reside.
 	 *
 	 * This page address is returned in order to be mapped onto the
 	 * TMP_DIR_MAPPING,etc reserved areas of physical memory.  This
 	 * defines the kernel's virtual address mapping.
 	 */
	unsigned long pg_dir = pg_pop();
	pde_t *dir = (pde_t *) pg_dir;
	pte_t *tbl;
	unsigned long phys, lin;
	unsigned long j;

 	/*
 	 * init one page directory entry for the system pages.  the
 	 * numbeer of them won't exceed 1024 pages, so only one is required.
 	 */
	pg_dir_new(dir);

 	/*
 	 * reserve each page of memory that the kernel requires
 	 */
	for (j = 0; j < sys_pgs; j++)
	{
		phys = lin = j * PG_SIZE;
 		/*
 		 * only the first directory entry will be initialized, so this
 		 * will only be true the first time around.
 		 */
		if (!dir[PDE(lin)].p)
		{
 			/*
 			 * set directory to be used,
 			 */
			dir[PDE(lin)].p = 1;
 			/* 
 			 * allocate 1 page for all of the table entries of this
 			 * directory.  set the directories sign_phys field to
 			 * point to the base address of these table entries.
 			 */
			dir[PDE(lin)].sign_phys = pg_pop() / PG_SIZE;
			tbl = (pte_t *) (dir[PDE(lin)].sign_phys * PG_SIZE);
			pg_tbl_new(tbl);
		}
		else
			tbl = (pte_t *) (dir[PDE(lin)].sign_phys * PG_SIZE);
 
 		/*
 		 * set table to used, and set its physical address.
 		 */
		tbl[PTE(lin)].p = 1;
		tbl[PTE(lin)].us = 0;
		tbl[PTE(lin)].sign_phys = phys / PG_SIZE;
	}

 	/*
 	 * TODO: WTF is this for?  It's apparently important.
 	 */
	lin = TBL_MAPPING;
	tbl = (pte_t *) (dir[PDE(lin)].sign_phys * PG_SIZE);
	tbl[PTE(lin)].sign_phys = dir[PDE(lin)].sign_phys;

	return pg_dir;
}

/*----------------------------------------------------------------------------*\
 |				 paging_init()				      |
\*----------------------------------------------------------------------------*/
void paging_init(unsigned long mem_upper)
{
	unsigned long sys_upper;
	unsigned long pg_dir;
	unsigned long j;

	num_pgs = (mem_upper *= KB) / PG_SIZE;
	/*
	 * sys_upper is the address where kernel heap pages will be based,
	 * and grow upwards from there.  The first few pages of the heap
	 * are the page directory and table entries.. see ascii art above
	 */
	sys_upper = PG_STACK_BASE + num_pgs * sizeof(pg_stack_t);
	sys_pgs = sys_upper / PG_SIZE + (sys_upper % PG_SIZE > 0);
	/*
	 * make a stack entry for every page the system has memory for.
	 */
	pg_stack_init();
	/*
	 * move all system pages to the used page stack.  the next pg_pop
	 * after this will occur at the bottom of the kernel heap.
	 */
	for (j = 0; j < sys_pgs; j++)
		pg_pop();

	/*
	 * reserve and init page dir/table entries for the system pages
	 * that were just popped.
	 */
	pg_dir = pg_space_create_init();
	/*
	 * store the kernel's PDE page address in the x86 cr3 register
	 */
	load_dir(pg_dir);
	enable_paging();
}

/*----------------------------------------------------------------------------*\
 |				pg_dir_tbl_map()			      |
\*----------------------------------------------------------------------------*/
unsigned long pg_dir_tbl_map(unsigned long phys, bool dir)
{
	pte_t *tbl = (pte_t *) TBL_MAPPING;
	unsigned long lin = dir ? TMP_DIR_MAPPING : TMP_TBL_MAPPING;
	unsigned long sign_phys = phys / PG_SIZE;

	if (tbl[PTE(lin)].sign_phys != sign_phys)
	{
		tbl[PTE(lin)].sign_phys = sign_phys;
		invalidate_tlbs(); /* translation lookaside buffer */
	}
	return lin;
}

/*----------------------------------------------------------------------------*\
 |				  pg_dir_map()				      |
\*----------------------------------------------------------------------------*/
pde_t *pg_dir_map(unsigned long pg_dir)
{
	return (pde_t *) pg_dir_tbl_map(pg_dir, PG_DIR);
}

/*----------------------------------------------------------------------------*\
 |				  pg_tbl_map()				      |
\*----------------------------------------------------------------------------*/
pte_t *pg_tbl_map(unsigned long pg_tbl)
{
	return (pte_t *) pg_dir_tbl_map(pg_tbl, PG_TBL);
}

/*----------------------------------------------------------------------------*\
 |				    pg_map()				      |
\*----------------------------------------------------------------------------*/
void pg_map(unsigned long pg_dir, unsigned long phys, unsigned long lin,
	unsigned char us)
{
	/*
	 * TODO: this returns TMP_DIR_MAPPING.. still don't know why.. 
	 */
	pde_t *dir = pg_dir_map(pg_dir);
	pte_t *tbl;

	if (!dir[PDE(lin)].p)
	{
		dir[PDE(lin)].p = 1;
		dir[PDE(lin)].sign_phys = pg_pop() / PG_SIZE;
		tbl = pg_tbl_map(dir[PDE(lin)].sign_phys * PG_SIZE);
		pg_tbl_new(tbl);
	}
	else
		tbl = pg_tbl_map(dir[PDE(lin)].sign_phys * PG_SIZE);
	tbl[PTE(lin)].p = 1;
	tbl[PTE(lin)].us = us;
	tbl[PTE(lin)].sign_phys = phys / PG_SIZE;

	if (pg_dir == save_dir())
		invalidate_tlbs();
}

/*----------------------------------------------------------------------------*\
 |				   pg_unmap()				      |
\*----------------------------------------------------------------------------*/
void pg_unmap(unsigned long pg_dir, unsigned long lin)
{
	pde_t *dir = pg_dir_map(pg_dir);
	pte_t *tbl = pg_tbl_map(dir[PDE(lin)].sign_phys * PG_SIZE);
	unsigned short j;
	bool found;

	/* set page to unused in it's table entry */
	tbl[PTE(lin)].p = 0;
	/*
	 * check if all pages in this table are unused.  if so, then
	 * put the table's page back on the free stack and set the
	 * directory entry to be unused.
	 */
	for (j = 0, found = false; j < 1024 && !found; j++)
		found = tbl[j].p;
	if (!found)
	{
		pg_push(dir[PDE(lin)].sign_phys * PG_SIZE);
		dir[PDE(lin)].p = 0;
	}

	if (pg_dir == save_dir())
		invalidate_tlbs();
}

/*----------------------------------------------------------------------------*\
 |			       pg_space_create()			      |
\*----------------------------------------------------------------------------*/
unsigned long pg_space_create(void)
{
	unsigned long pg_dir = pg_pop();
	pde_t *dir = pg_dir_map(pg_dir); /* !... returns TMP_DIR_MAPPING? */
	pte_t *tbl;
	unsigned long phys, lin;
	unsigned long j;

	/* 
	 * called from task_create, it sets up the tasks page tables
	 */
	pg_dir_new(dir);

	/*
	 * map the systems pages directly onto the tasks pages
	 */
	for (j = 0; j < sys_pgs; j++)
	{
		phys = lin = j * PG_SIZE;
		pg_map(pg_dir, phys, lin, 0);
	}

	/* I've forgotten what this is for, but it looks important. */
	lin = TBL_MAPPING;
	tbl = pg_tbl_map(dir[PDE(lin)].sign_phys * PG_SIZE);
	tbl[PTE(lin)].sign_phys = dir[PDE(lin)].sign_phys;

	return pg_dir;
}

/*----------------------------------------------------------------------------*\
 |			       pg_space_destroy()			      |
\*----------------------------------------------------------------------------*/
void pg_space_destroy(unsigned long pg_dir)
{
	pde_t *dir = pg_dir_map(pg_dir);
	pte_t *tbl;
	unsigned short j, k;

	/*
	 * TODO: I don't understand this.. why 256?  why directory entries.
	 */
	for (j = 0; j < 256; j++)
	{
		if (!dir[j].p)
			continue;
		pg_push(dir[j].sign_phys * PG_SIZE);
	}
	for (j = 256; j < 1024; j++)
	{
		if (!dir[j].p)
			continue;
		/*
		 * TODO: ditto, maybe translation lookaside that I don't
		 * understand
		 */
		tbl = pg_tbl_map(dir[j].sign_phys * PG_SIZE);
		for (k = 0; k < 1024; k++)
		{
			if (!tbl[k].p)
				continue;
			pg_push(tbl[k].sign_phys * PG_SIZE);
		}
		pg_push(dir[j].sign_phys * PG_SIZE);
	}
	pg_push(pg_dir);
}

/*----------------------------------------------------------------------------*\
 |				   pg_addr()				      |
\*----------------------------------------------------------------------------*/
unsigned long pg_addr(bool kernel, bool heap, bool bottom)
{
	if (kernel)
		if (heap)
			return bottom ? KERNEL_HEAP_BOTTOM : KERNEL_HEAP_TOP;
		else
			return bottom ? KERNEL_STACK_BOTTOM : KERNEL_STACK_TOP;
	else
		if (heap)
			return bottom ? USER_HEAP_BOTTOM : USER_HEAP_TOP;
		else
			return bottom ? USER_STACK_BOTTOM : USER_STACK_TOP;
}

/*----------------------------------------------------------------------------*\
 |				  pg_mapped()				      |
\*----------------------------------------------------------------------------*/
bool pg_mapped(unsigned long pg_dir, unsigned long lin)
{
	pde_t *dir = pg_dir_map(pg_dir);
	pte_t *tbl;

	if (!dir[PDE(lin)].p)
		return false;
	tbl = pg_tbl_map(dir[PDE(lin)].sign_phys * PG_SIZE);
	if (!tbl[PTE(lin)].p)
		return false;
	return true;
}

/*----------------------------------------------------------------------------*\
 |				 pg_next_addr()				      |
\*----------------------------------------------------------------------------*/
unsigned long pg_next_addr(unsigned long pg_dir, bool kernel, bool heap)
{
	unsigned long vague_lin = kernel ?
		(heap ? KERNEL_HEAP_BOTTOM : KERNEL_STACK_TOP - PG_SIZE) :
		(heap ?   USER_HEAP_BOTTOM :   USER_STACK_TOP - PG_SIZE);
	lin_addr_t *descr_lin = (lin_addr_t *) &vague_lin;

	while (pg_mapped(pg_dir, vague_lin))
	{
		/*
		 * if the next table entry hits the end of the table depending
		 * on which way it is growing, increment the directory entry,
		 * and keep search for an unused page.
		 */
		if (descr_lin->pte == heap ? 1023 : 0)
		{
			descr_lin->pde += heap ? 1 : -1;
			descr_lin->pte = heap ? 0 : 1023;
		}
		else
			descr_lin->pte += heap ? 1 : -1;
	}
	return vague_lin;
}

/*----------------------------------------------------------------------------*\
 |				   pg_alloc()				      |
\*----------------------------------------------------------------------------*/
unsigned long pg_alloc(unsigned long pg_dir, bool kernel, bool heap)
{
	unsigned long phys = pg_pop();
	unsigned long lin = pg_next_addr(pg_dir, kernel, heap);
	pg_map(pg_dir, phys, lin, !kernel);
	return lin;
}

/*----------------------------------------------------------------------------*\
 |				   pg_free()				      |
\*----------------------------------------------------------------------------*/
void pg_free(unsigned long pg_dir, unsigned long lin)
{
	pde_t *dir = pg_dir_map(pg_dir);
	pte_t *tbl = pg_tbl_map(dir[PDE(lin)].sign_phys * PG_SIZE);
	unsigned long phys = tbl[PTE(lin)].sign_phys * PG_SIZE;
	pg_unmap(pg_dir, lin);
	/*
	 * TODO: what if some other process has this page mapped?
	 * 		I can't find this function being used anywhere
	 */
	pg_push(phys);
}

/*----------------------------------------------------------------------------*\
 |				   pg_copy()				      |
\*----------------------------------------------------------------------------*/
void pg_copy(unsigned long phys_1, unsigned long phys_2)
{
	void *p1, *p2;

	intr_lock();
	/*
	 * TODO: ones dir_map the other is tbl_map..  I'm guessing because
	 * you can only have one mapped at a time?
	 */
	p1 = pg_dir_map(phys_1);
	p2 = pg_tbl_map(phys_2);
	memcpy(p2, p1, PG_SIZE);
	intr_unlock();
}

/*----------------------------------------------------------------------------*\
 |				   pg_fork()				      |
\*----------------------------------------------------------------------------*/
void pg_fork(unsigned long pg_dir_1, unsigned long pg_dir_2)
{
	pde_t *dir;
	pte_t *tbl;
	unsigned long phys_1, phys_2, vague_lin;
	lin_addr_t *descr_lin = (lin_addr_t *) &vague_lin;
	unsigned short j, k;

	/*
	 * TODO it's usless to understand this until the dir/table mapping is
	 * understood.
	 */
	dir = pg_dir_map(pg_dir_2);
	for (j = 256; j < 1024; j++)
	{
		if (!dir[j].p)
			continue;
		tbl = pg_tbl_map(dir[j].sign_phys * PG_SIZE);
		for (k = 0; k < 1024; k++)
		{
			if (!tbl[k].p)
				continue;
			pg_push(tbl[k].sign_phys * PG_SIZE);
		}
		pg_push(dir[j].sign_phys * PG_SIZE);
	}

	dir = pg_dir_map(pg_dir_1);
	for (j = 256; j < 1024; j++)
	{
		if (!dir[j].p)
			continue;
		tbl = pg_tbl_map(dir[j].sign_phys * PG_SIZE);
		for (k = 0; k < 1024; k++)
		{
			if (!tbl[k].p)
				continue;
			phys_1 = tbl[k].sign_phys * PG_SIZE;
			phys_2 = pg_pop();
			pg_copy(phys_1, phys_2);
			descr_lin->pde = j;
			descr_lin->pte = k;
			pg_map(pg_dir_2, phys_2, vague_lin, USER);
			dir = pg_dir_map(pg_dir_1);
			tbl = pg_tbl_map(dir[j].sign_phys * PG_SIZE);
		}
	}
}

/*----------------------------------------------------------------------------*\
 |				   pg_exec()				      |
\*----------------------------------------------------------------------------*/
void pg_exec(unsigned long pg_dir_1, unsigned long pg_dir_2)
{
	pde_t *dir;
	pte_t *tbl;
	unsigned long phys, vague_lin;
	lin_addr_t *descr_lin = (lin_addr_t *) &vague_lin;
	unsigned short j, k;

	dir = pg_dir_map(pg_dir_2);
	for (j = 512; j < 768; j++)
	{
		if (!dir[j].p)
			continue;
		tbl = pg_tbl_map(dir[j].sign_phys * PG_SIZE);
		for (k = 0; k < 1024; k++)
		{
			if (!tbl[k].p)
				continue;
			pg_push(tbl[k].sign_phys * PG_SIZE);
		}
		pg_push(dir[j].sign_phys * PG_SIZE);
	}

	dir = pg_dir_map(pg_dir_1);
	for (j = 512; j < 768; j++)
	{
		if (!dir[j].p)
			continue;
		tbl = pg_tbl_map(dir[j].sign_phys * PG_SIZE);
		for (k = 0; k < 1024; k++)
		{
			if (!tbl[k].p)
				continue;
			phys = tbl[k].sign_phys * PG_SIZE;
			descr_lin->pde = j;
			descr_lin->pte = k;
			pg_map(pg_dir_2, phys, vague_lin, USER);
			pg_unmap(pg_dir_1, vague_lin);
		}
	}

	heap_init(pg_dir_1, USER);
}
