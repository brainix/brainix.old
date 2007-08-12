/*----------------------------------------------------------------------------*\
 |	irq.c - 8259A PIC (Programmable Interrupt Controller) code	      |
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

/* Registers: */
#define MASTER_CTRL	0x20 /* Master PIC's control register.        */
#define MASTER_IMR	0x21 /* Master PIC's interrupt mask register. */
#define SLAVE_CTRL	0xA0 /* Slave PIC's control register.         */
#define SLAVE_IMR	0xA1 /* Slave PIC's interrupt mask register.  */

/* Master PIC's initialization command words: */
#define MASTER_ICW_1	0x11 /* Initialization command word 4 needed. */
#define MASTER_ICW_2	0x20 /* IRQs 0-7 mapped to interrupts 32-39.  */
#define MASTER_ICW_3	0x04 /* Slave cascaded on IRQ 2.              */
#define MASTER_ICW_4	0x05 /* Non-buffered, normal EOI, 8088.       */

/* Slave PIC's initialization command words: */
#define SLAVE_ICW_1	0x11 /* Initialization command word 4 needed. */
#define SLAVE_ICW_2	0x28 /* IRQs 8-15 mapped to interrupts 40-47. */
#define SLAVE_ICW_3	0x02 /* Slave ID: 2.                          */
#define SLAVE_ICW_4	0x01 /* Non-buffered, normal EOI, 8088.       */

#define EOI		0x20 /* */

/* Global variables: */
unsigned char master_ocw_1 = 0xFF; /* Master PIC's operation control word 1. */
unsigned char slave_ocw_1 = 0xFF;  /* Slave PIC's operation control word 1.  */
pid_t irq_to_pid[16];

/* Function prototypes: */
void irq_disable(unsigned char num);
void irq_enable(unsigned char num);
void irq_init(void);
void irq_eoi(unsigned char num);
void irq_register(msg_t *msg);
void irq_handler(unsigned char num);

/*----------------------------------------------------------------------------*\
 |				 irq_disable()				      |
\*----------------------------------------------------------------------------*/
void irq_disable(unsigned char num)
{

/* Disable an IRQ. */

	intr_lock();
	if (num <= 7)
		out_byte(master_ocw_1 |= 1 << num, MASTER_IMR);
	if (num >= 8 && num <= 15)
		out_byte(slave_ocw_1 |= 1 << (num - 8), SLAVE_IMR);
	intr_unlock();
}

/*----------------------------------------------------------------------------*\
 |				  irq_enable()				      |
\*----------------------------------------------------------------------------*/
void irq_enable(unsigned char num)
{

/* Enable an IRQ. */

	intr_lock();
	if (num <= 7)
		out_byte(master_ocw_1 &= ~(1 << num), MASTER_IMR);
	if (num >= 8 && num <= 15)
		out_byte(slave_ocw_1 &= ~(1 << (num - 8)), SLAVE_IMR);
	intr_unlock();
}

/*----------------------------------------------------------------------------*\
 |				   irq_init()				      |
\*----------------------------------------------------------------------------*/
void irq_init(void)
{

/* Initialize IRQs. */

	/* Initialize the master PIC. */
	out_byte(MASTER_ICW_1, MASTER_CTRL);
	out_byte(MASTER_ICW_2, MASTER_IMR);
	out_byte(MASTER_ICW_3, MASTER_IMR);
	out_byte(MASTER_ICW_4, MASTER_IMR);

	/* Initialize the slave PIC. */
	out_byte(SLAVE_ICW_1, SLAVE_CTRL);
	out_byte(SLAVE_ICW_2, SLAVE_IMR);
	out_byte(SLAVE_ICW_3, SLAVE_IMR);
	out_byte(SLAVE_ICW_4, SLAVE_IMR);

	/* Disable all IRQs. */
	out_byte(master_ocw_1, MASTER_IMR);
	out_byte(slave_ocw_1, SLAVE_IMR);

	intr_set(32, (unsigned long) wrapper_32, KERNEL_PL);
	intr_set(33, (unsigned long) wrapper_33, KERNEL_PL);
	intr_set(34, (unsigned long) wrapper_34, KERNEL_PL);
	intr_set(35, (unsigned long) wrapper_35, KERNEL_PL);
	intr_set(36, (unsigned long) wrapper_36, KERNEL_PL);
	intr_set(37, (unsigned long) wrapper_37, KERNEL_PL);
	intr_set(38, (unsigned long) wrapper_38, KERNEL_PL);
	intr_set(39, (unsigned long) wrapper_39, KERNEL_PL);
	intr_set(40, (unsigned long) wrapper_40, KERNEL_PL);
	intr_set(41, (unsigned long) wrapper_41, KERNEL_PL);
	intr_set(42, (unsigned long) wrapper_42, KERNEL_PL);
	intr_set(43, (unsigned long) wrapper_43, KERNEL_PL);
	intr_set(44, (unsigned long) wrapper_44, KERNEL_PL);
	intr_set(45, (unsigned long) wrapper_45, KERNEL_PL);
	intr_set(46, (unsigned long) wrapper_46, KERNEL_PL);
	intr_set(47, (unsigned long) wrapper_47, KERNEL_PL);

	intr_unlock();
}

/*----------------------------------------------------------------------------*\
 |				   irq_eoi()				      |
\*----------------------------------------------------------------------------*/
void irq_eoi(unsigned char num)
{
	if (num <= 15)
	{
		if (num >= 8)
			out_byte(EOI, SLAVE_CTRL);
		out_byte(EOI, MASTER_CTRL);
	}
}

/*----------------------------------------------------------------------------*\
 |				 irq_register()				      |
\*----------------------------------------------------------------------------*/
void irq_register(msg_t *msg)
{
	irq_to_pid[msg->args.brnx_reg.irq] = msg->from;
	irq_enable(msg->args.brnx_reg.irq);
}

/*----------------------------------------------------------------------------*\
 |				 irq_handler()				      |
\*----------------------------------------------------------------------------*/
void irq_handler(unsigned char num)
{
	pid_t pid = irq_to_pid[num];
	msg_t *msg = msg_alloc(pid, IRQ);
	msg->from = HARDWARE;
	irq_eoi(num);
	msg_send(msg);
}
