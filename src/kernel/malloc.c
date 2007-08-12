/*----------------------------------------------------------------------------*\
 |	malloc.c							      |
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

/* Return the address of the first slab. */
#define SLAB_FIRST(kernel) \
	((slab_t *) pg_addr(kernel, HEAP, BOTTOM))

/* Return the address of the last slab. */
#define SLAB_LAST(kernel) \
	(SLAB_FIRST(kernel)->prev)

/* Return the address of where the next slab should go. */
#define SLAB_NEXT(pg_dir, kernel) \
	(pg_next_addr(pg_dir, kernel, HEAP))

/* Allocate a 4 KB page for a new slab and return its address. */
#define SLAB_NEW(pg_dir, kernel) \
	((slab_t *) pg_alloc(pg_dir, kernel, HEAP))

/* Data structure: */
typedef struct slab
{
	bool used;
	size_t size;
	struct slab *prev;
	struct slab *next;
} __attribute__((packed)) slab_t;

/* Global variable: */
unsigned long reference_pg_dir;

/* Function prototypes: */
bool heap_pg_fault(bool kernel, unsigned long lin);
bool heap_init(unsigned long target_pg_dir, bool kernel);
void *common_sbrk(bool kernel, ptrdiff_t increment);
void split(slab_t *slab, size_t size);
void merge(slab_t *slab);
void *common_malloc(bool kernel, size_t size);
void common_free(bool kernel, void *ptr);
void *ksbrk(ptrdiff_t increment);
void *do_sbrk(ptrdiff_t increment);
void *kmalloc(size_t size);
void *do_malloc(size_t size);
void kfree(void *ptr);
void do_free(void *ptr);

/* Function prototypes in paging.c: */
pde_t *pg_dir_map(unsigned long pg_dir);
pte_t *pg_tbl_map(unsigned long pg_tbl);
void pg_map(unsigned long pg_dir, unsigned long phys, unsigned long lin,
	unsigned char us);
void pg_unmap(unsigned long pg_dir, unsigned long lin);

/*----------------------------------------------------------------------------*\
 |				heap_pg_fault()				      |
\*----------------------------------------------------------------------------*/
bool heap_pg_fault(bool kernel, unsigned long lin)
{

/* A page-fault exception has occurred in heap space.  Try to handle it.  Return
 * true on success or false on failure. */

	pde_t *dir;
	pte_t *tbl;
	unsigned long pg_dir;
	unsigned long phys;
	unsigned char us;

	if (!kernel)
		/* Oops.  The page-fault exception occurred in user heap space.
		 * Brainix doesn't (yet) support swapping, so this can only be
		 * the result of shitty programming.  Vomit. */
		return false;

	dir = pg_dir_map(reference_pg_dir);
	if (!dir[PDE(lin)].p)
		return false;
	tbl = pg_tbl_map(dir[PDE(lin)].sign_phys * PG_SIZE);
	if (!tbl[PTE(lin)].p)
		return false;
	pg_dir = save_dir();
	phys = tbl[PTE(lin)].sign_phys * PG_SIZE;
	us = tbl[PTE(lin)].us;
	pg_map(pg_dir, phys, lin, us);
	return true;
}

/*----------------------------------------------------------------------------*\
 |				  heap_init()				      |
\*----------------------------------------------------------------------------*/
bool heap_init(unsigned long target_pg_dir, bool kernel)
{

/* In the specified virtual address space, initialize either the kernel or user
 * heap.  Return true on success or false on failure. */

	unsigned long current_pg_dir;
	slab_t *slab;
	bool ret_val;

	current_pg_dir = save_dir();
	if (kernel)
		reference_pg_dir = target_pg_dir;
	else if (target_pg_dir != current_pg_dir)
		load_dir(target_pg_dir);

	/*
	 * this will allocate a page right after the systems dir and tbl
	 * entries on the heap.  It's important to note that the VMM is
	 * keeping the address spaces from colliding with each other.
	 */
	if ((slab = SLAB_NEW(target_pg_dir, kernel)) == NULL)
		ret_val = false;
	else
	{
		slab->used = false;
		slab->size = PG_SIZE - sizeof(slab_t);
		slab->next = slab->prev = slab;
		ret_val = true;
	}

	if (!kernel && target_pg_dir != current_pg_dir)
		load_dir(current_pg_dir);
	return ret_val;
}

/*----------------------------------------------------------------------------*\
 |				 common_sbrk()				      |
\*----------------------------------------------------------------------------*/
void *common_sbrk(bool kernel, ptrdiff_t increment)
{
	unsigned long pg_dir = kernel ? reference_pg_dir : save_dir();
	unsigned long old_break;
	unsigned long num_new_pgs;
	slab_t *slab;
	unsigned long j;

	old_break = SLAB_NEXT(pg_dir, kernel);
	if (increment <= 0)
		return (void *) old_break;

	increment += sizeof(slab_t);
	num_new_pgs = increment / PG_SIZE + (increment % PG_SIZE > 0);
	for (j = 0; j < num_new_pgs; j++)
		if (SLAB_NEW(reference_pg_dir, kernel) == NULL)
		{
			errno = ENOMEM;
			return (void *) -1;
		}

	slab = SLAB_LAST(kernel);
	(slab->next = (slab_t *) old_break)->prev = slab;
	(slab = slab->next)->used = false;
	slab->size = num_new_pgs * PG_SIZE - sizeof(slab_t);
	(slab->next = SLAB_FIRST(kernel))->prev = slab;

	return (char *) slab + sizeof(slab_t);
}

/*----------------------------------------------------------------------------*\
 |				    split()				      |
\*----------------------------------------------------------------------------*/
void split(slab_t *slab, size_t size)
{

/* Split a slab into two slabs.  This is done to reduce wastage. */

	slab_t *new_slab = (slab_t *) ((char *) slab + sizeof(slab_t) + size);
	new_slab->used = false;
	new_slab->size = slab->size - (size + sizeof(slab_t));
	new_slab->prev = slab;
	new_slab->next = slab->next;
	slab->size = size;
	slab->next = slab->next->prev = new_slab;
}

/*----------------------------------------------------------------------------*\
 |				    merge()				      |
\*----------------------------------------------------------------------------*/
void merge(slab_t *slab)
{

/* Merge a slab with the next slab.  This is done to reduce fragmentation. */

	slab->size += sizeof(slab_t) + slab->next->size;
	(slab->next = slab->next->next)->prev = slab;
}

/*----------------------------------------------------------------------------*\
 |				common_malloc()				      |
\*----------------------------------------------------------------------------*/
void *common_malloc(bool kernel, size_t size)
{
	slab_t *slab = SLAB_FIRST(kernel);

	if (size == 0)
		/* Some douche is trying to allocate 0 bytes.  Return NULL. */
		return NULL;

	for (; slab->used || slab->size < size; slab = slab->next)
		if (slab->next == SLAB_FIRST(kernel))
			common_sbrk(kernel, size);

	if (slab->size > size + sizeof(slab_t))
		split(slab, size);

	slab->used = true;
	return (char *) slab + sizeof(slab_t);
}

/*----------------------------------------------------------------------------*\
 |				 common_free()				      |
\*----------------------------------------------------------------------------*/
void common_free(bool kernel, void *ptr)
{
	slab_t *slab;

	if (ptr == NULL)
		/* Some douche is trying to free NULL.  Return. */
		return;

	slab = (slab_t *) ((char *) ptr - sizeof(slab_t));
	slab->used = false;

	if (slab->next != SLAB_FIRST(kernel) && !slab->next->used)
		merge(slab);
	if (slab->prev != SLAB_LAST(kernel) && !slab->prev->used)
		merge(slab->prev);
}

/*----------------------------------------------------------------------------*\
 |				    ksbrk()				      |
\*----------------------------------------------------------------------------*/
void *ksbrk(ptrdiff_t increment)
{
	void *ptr;

	intr_lock();
	ptr = common_sbrk(KERNEL, increment);
	intr_unlock();
	return ptr;
}

/*----------------------------------------------------------------------------*\
 |				   do_sbrk()				      |
\*----------------------------------------------------------------------------*/
void *do_sbrk(ptrdiff_t increment)
{
	void *ptr;

	intr_lock();
	ptr = common_sbrk(USER, increment);
	intr_unlock();
	return ptr;
}

/*----------------------------------------------------------------------------*\
 |				   kmalloc()				      |
\*----------------------------------------------------------------------------*/
void *kmalloc(size_t size)
{
	void *ptr;

	intr_lock();
	ptr = common_malloc(KERNEL, size);
	intr_unlock();
	return ptr;
}

/*----------------------------------------------------------------------------*\
 |				  do_malloc()				      |
\*----------------------------------------------------------------------------*/
void *do_malloc(size_t size)
{
	void *ptr;

	intr_lock();
	ptr = common_malloc(USER, size);
	intr_unlock();
	return ptr;
}

/*----------------------------------------------------------------------------*\
 |				    kfree()				      |
\*----------------------------------------------------------------------------*/
void kfree(void *ptr)
{
	intr_lock();
	common_free(KERNEL, ptr);
	intr_unlock();
}

/*----------------------------------------------------------------------------*\
 |				   do_free()				      |
\*----------------------------------------------------------------------------*/
void do_free(void *ptr)
{
	intr_lock();
	common_free(USER, ptr);
	intr_unlock();
}
