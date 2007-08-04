/*----------------------------------------------------------------------------*\
 |	struct.h							      |
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

#ifndef _KERNEL_STRUCT_H
#define _KERNEL_STRUCT_H

/* Protection rings: */
#define KERNEL_PL	0 /* Operating system kernel.   */
#define SERVICE_1_PL	1 /* Operating system services. */
#define SERVICE_2_PL	2 /* Operating system services. */
#define USER_PL		3 /* Applications.              */

/* Segment selector: */
typedef struct
{
	unsigned rpl   :  2; /* Requested privilege level. */
	unsigned ti    :  1; /* Table indicator.           */
	unsigned index : 13; /* Index.                     */
} __attribute__((packed)) seg_sel_t;

#define GDT	0 /* Global descriptor table. */
#define LDT	1 /* Local descriptor table.  */

/* Logical address: */
typedef struct
{
	unsigned long offset; /* Offset.           */
	seg_sel_t seg_sel;    /* Segment selector. */
} __attribute__((packed)) log_addr_t;

/* Segment descriptor: */
typedef struct
{
	unsigned short limit_0; /* Segment limit (15:00).        */
	unsigned short base_0;  /* Segment base address (15:00). */
	unsigned char base_1;   /* Segment base address (23:16). */
	unsigned type     : 4;  /* Segment type.                 */
	unsigned s        : 1;  /* Descriptor type.              */
	unsigned dpl      : 2;  /* Descriptor privilege level.   */
	unsigned p        : 1;  /* Segment present.              */
	unsigned limit_1  : 4;  /* Segment limit (19:16).        */
	unsigned avl      : 1;  /* Available for use.            */
	unsigned reserved : 1;  /* Reserved.                     */
	unsigned db       : 1;  /* Default operation size.       */
	unsigned g        : 1;  /* Granularity.                  */
	unsigned char base_2;   /* Segment base address (31:24). */
} __attribute__((packed)) seg_descr_t;

#define DATA_SEG_TYPE(expansion_direction, write_enable, accessed) \
	((expansion_direction << 2) | (write_enable << 1) | accessed)

#define DATA_READ_ONLY				DATA_SEG_TYPE(0, 0, 0)
#define DATA_READ_ONLY_ACCESSED			DATA_SEG_TYPE(0, 0, 1)
#define DATA_READ_WRITE				DATA_SEG_TYPE(0, 1, 0)
#define DATA_READ_WRITE_ACCESSED		DATA_SEG_TYPE(0, 1, 1)
#define DATA_READ_ONLY_EXPAND_DOWN		DATA_SEG_TYPE(1, 0, 0)
#define DATA_READ_ONLY_EXPAND_DOWN_ACCESSED	DATA_SEG_TYPE(1, 0, 1)
#define DATA_READ_WRITE_EXPAND_DOWN		DATA_SEG_TYPE(1, 1, 0)
#define DATA_READ_WRITE_EXPAND_DOWN_ACCESSED	DATA_SEG_TYPE(1, 1, 1)

#define CODE_SEG_TYPE(conforming, read_enable, accessed) \
	(0xF | (conforming << 2) | (read_enable << 1) | accessed)

#define CODE_EXEC_ONLY				CODE_SEG_TYPE(0, 0, 0)
#define CODE_EXEC_ONLY_ACCESSED			CODE_SEG_TYPE(0, 0, 1)
#define CODE_EXEC_READ				CODE_SEG_TYPE(0, 1, 0)
#define CODE_EXEC_READ_ACCESSED			CODE_SEG_TYPE(0, 1, 1)
#define CODE_EXEC_ONLY_CONFORMING		CODE_SEG_TYPE(1, 0, 0)
#define CODE_EXEC_ONLY_CONFORMING_ACCESSED	CODE_SEG_TYPE(1, 0, 1)
#define CODE_EXEC_READ_ONLY_CONFORMING		CODE_SEG_TYPE(1, 1, 0)
#define CODE_EXEC_READ_ONLY_CONFORMING_ACCESSED	CODE_SEG_TYPE(1, 1, 1)

#define SYSTEM_GATE_TSS_16_AVAILABLE	0x1
#define SYSTEM_GATE_LDT			0x2
#define SYSTEM_GATE_TSS_16_BUSY		0x3
#define SYSTEM_GATE_CALL_GATE_16	0x4
#define SYSTEM_GATE_TASK_GATE		0x5
#define SYSTEM_GATE_INTR_GATE_16	0x6
#define SYSTEM_GATE_TRAP_GATE_16	0x7
#define SYSTEM_GATE_TSS_32_AVAILABLE	0x9
#define SYSTEM_GATE_TSS_32_BUSY		0xB
#define SYSTEM_GATE_CALL_GATE_32	0xC
#define SYSTEM_GATE_INTR_GATE_32	0xE
#define SYSTEM_GATE_TRAP_GATE_32	0xF

#define SYSTEM		0
#define CODE_OR_DATA	1

#define DEFAULT_OPERATION_16	0
#define DEFAULT_OPERATION_32	1

#define GRANULARITY_1B	0
#define GRANULARITY_4K	1

/* Pseudo-descriptor: */
typedef struct
{
	unsigned short limit; /* Limit.        */
	unsigned long base;   /* Base address. */
} __attribute__((packed)) pseudo_descr_t;

/* IDT gate descriptor: */
typedef struct
{
	unsigned short offset_0; /* Offset (15:00).             */
	seg_sel_t seg_sel;       /* Segment selector.           */
	unsigned reserved_0 : 5; /* Reserved.                   */
	unsigned reserved_1 : 3; /* Reserved.                   */
	unsigned gate_type  : 5; /* Gate type and size.         */
	unsigned dpl        : 2; /* Descriptor privilege level. */
	unsigned p          : 1; /* Segment present.            */
	unsigned short offset_1; /* Offset (31:16).             */
} __attribute__((packed)) idt_descr_t;

#define TASK_GATE	0x05
#define INTR_GATE_16	0x06
#define INTR_GATE_32	0x0E
#define TRAP_GATE_16	0x07
#define TRAP_GATE_32	0x0F

/* Control register 3: */
typedef struct
{
	unsigned reserved_0 :  3; /* Reserved.                      */
	unsigned pwt        :  1; /* Page-level writes transparent. */
	unsigned pcd        :  1; /* Page-level cache disable.      */
	unsigned reserved_1 :  7; /* Reserved.                      */
	unsigned sign_phys  : 20; /* Page-directory base.           */
} __attribute__((packed)) cr3_t;

/* Control register 0: */
typedef struct
{
	unsigned pe         :  1; /* Protection enable.   */
	unsigned mp         :  1; /* Monitor coprocessor. */
	unsigned em         :  1; /* Emulation.           */
	unsigned ts         :  1; /* Task switched.       */
	unsigned et         :  1; /* Extension type.      */
	unsigned ne         :  1; /* Numeric error.       */
	unsigned reserved_0 : 10; /* Reserved.            */
	unsigned wp         :  1; /* Write protect.       */
	unsigned reserved_1 :  1; /* Reserved.            */
	unsigned am         :  1; /* Alignment mask.      */
	unsigned reserved_2 : 10; /* Reserved.            */
	unsigned nw         :  1; /* Not write-through.   */
	unsigned cd         :  1; /* Cache disable.       */
	unsigned pg         :  1; /* Paging.              */
} __attribute__((packed)) cr0_t;

/* Page-directory entry: */
typedef struct
{
	unsigned p         :  1; /* Present.                  */
	unsigned rw        :  1; /* Read/write.               */
	unsigned us        :  1; /* User/supervisor.          */
	unsigned pwt       :  1; /* Page-level write-through. */
	unsigned pcd       :  1; /* Page-level cache disable. */
	unsigned a         :  1; /* Accessed.                 */
	unsigned reserved  :  1; /* Reserved.                 */
	unsigned ps        :  1; /* Page size.                */
	unsigned g         :  1; /* Global page.              */
	unsigned avl       :  3; /* Available for use.        */
	unsigned sign_phys : 20; /* Page-table base address.  */
} __attribute__((packed)) pde_t;

/* Page-table entry: */
typedef struct
{
	unsigned p         :  1; /* Present.                    */
	unsigned rw        :  1; /* Read/write.                 */
	unsigned us        :  1; /* User/supervisor.            */
	unsigned pwt       :  1; /* Page-level write-through.   */
	unsigned pcd       :  1; /* Page-level cache disable.   */
	unsigned a         :  1; /* Accessed.                   */
	unsigned d         :  1; /* Dirty.                      */
	unsigned pat       :  1; /* Page attribute table index. */
	unsigned g         :  1; /* Global page.                */
	unsigned avl       :  3; /* Available for use.          */
	unsigned sign_phys : 20; /* Page base address.          */
} __attribute__((packed)) pte_t;

/* Linear address: */
typedef struct
{
	unsigned offset : 12; /* Page offset.          */
	unsigned pte    : 10; /* Page-table entry.     */
	unsigned pde    : 10; /* Page-directory entry. */
} __attribute__((packed)) lin_addr_t;

#define PG_SIZE	(4 * KB)

/*
 * 1024 page directory entries (PDE)
 * 1024 page table entries (PTE)
 *
 * these calculate the offset into each table
 * when given the linear address.
 */
#define PDE(lin)	((lin & 0xFFC00000) >> 22)
#define PTE(lin)	((lin & 0x003FF000) >> 12)
#define OFFSET(lin)	((lin & 0x00000FFF) >> 0)

/* EFLAGS register: */
typedef struct
{
	unsigned cf         :  1; /* Carry flag.                     */
	unsigned reserved_0 :  1; /* Reserved.                       */
	unsigned pf         :  1; /* Parity flag.                    */
	unsigned reserved_1 :  1; /* Reserved.                       */
	unsigned af         :  1; /* Auxiliary carry flag.           */
	unsigned reserved_2 :  1; /* Reserved.                       */
	unsigned zf         :  1; /* Zero flag.                      */
	unsigned sf         :  1; /* Sign flag.                      */
	unsigned tf         :  1; /* Trap flag.                      */
	unsigned intrf      :  1; /* Interrupt enable flag.          */
	unsigned df         :  1; /* Direction flag.                 */
	unsigned of         :  1; /* Overflow flag.                  */
	unsigned iopl       :  2; /* I/O privilege level field.      */
	unsigned nt         :  1; /* Nested task flag.               */
	unsigned reserved_3 :  1; /* Reserved.                       */
	unsigned rf         :  1; /* Resume flag.                    */
	unsigned vm         :  1; /* Virtual-8086 mode flag.         */
	unsigned ac         :  1; /* Alignment check flag.           */
	unsigned vif        :  1; /* Virtual interrupt flag.         */
	unsigned vip        :  1; /* Virtual interrupt pending flag. */
	unsigned id         :  1; /* Identification flag.            */
	unsigned reserved_4 : 10; /* Reserved.                       */
} __attribute__((packed)) eflags_t;

/* Task-state segment: */
typedef struct
{
	seg_sel_t prev_task_link;   /* Previous task link field. */
	unsigned short reserved_0;  /* Reserved.                 */

	/* Privilege level-0, -1, and -2 stack pointer fields: */
	unsigned long esp0;         /* ESP0.     */
	seg_sel_t ss0;              /* SS0.      */
	unsigned short reserved_1;  /* Reserved. */
	unsigned long esp1;         /* ESP1.     */
	seg_sel_t ss1;              /* SS1.      */
	unsigned short reserved_2;  /* Reserved. */
	unsigned long esp2;         /* ESP2.     */
	seg_sel_t ss2;              /* SS2.      */
	unsigned short reserved_3;  /* Reserved. */

	cr3_t cr3;                  /* CR3 control register field.      */
	unsigned long eip;          /* EIP (instruction pointer) field. */
	eflags_t eflags;            /* EFLAGS register field.           */

	/* General-purpose register fields: */
	unsigned long eax;          /* EAX. */
	unsigned long ecx;          /* ECX. */
	unsigned long edx;          /* EDX. */
	unsigned long ebx;          /* EBX. */
	unsigned long esp;          /* ESP. */
	unsigned long ebp;          /* EBP. */
	unsigned long esi;          /* ESI. */
	unsigned long edi;          /* EDI. */

	/* Segment selector fields: */
	seg_sel_t es;               /* ES.       */
	unsigned short reserved_4;  /* Reserved. */
	seg_sel_t cs;               /* CS.       */
	unsigned short reserved_5;  /* Reserved. */
	seg_sel_t ss;               /* SS.       */
	unsigned short reserved_6;  /* Reserved. */
	seg_sel_t ds;               /* DS.       */
	unsigned short reserved_7;  /* Reserved. */
	seg_sel_t fs;               /* FS.       */
	unsigned short reserved_8;  /* Reserved. */
	seg_sel_t gs;               /* GS.       */
	unsigned short reserved_9;  /* Reserved. */

	seg_sel_t ldt_seg_sel;      /* LDT segment selector field. */
	unsigned short reserved_10; /* Reserved.                   */
	unsigned t           :  1;  /* T (debug trap) flag.        */
	unsigned reserved_11 : 15;  /* Reserved.                   */
	unsigned short io_map;      /* I/O map base address field. */
} __attribute__((packed)) tss_t;

/* Stack layout after PUSHAD: */
typedef struct
{
	unsigned long eax; /* EAX.                  */
	unsigned long ecx; /* ECX.                  */
	unsigned long edx; /* EDX.                  */
	unsigned long ebx; /* EBX.                  */
	unsigned long esp; /* ESP (original value). */
	unsigned long ebp; /* EBP.                  */
	unsigned long esi; /* ESI.                  */
	unsigned long edi; /* EDI.                  */
} __attribute__((packed)) stack_t;

#endif
