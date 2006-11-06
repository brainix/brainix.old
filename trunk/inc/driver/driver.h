/*----------------------------------------------------------------------------*\
 |	driver.h							      |
 |									      |
 |	Copyright © 2002-2006, Team Brainix, original authors.		      |
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

#ifndef _DRIVER_H
#define _DRIVER_H

#include <kernel/kernel.h>
#include <stdbool.h>

typedef struct
{
	msg_t **msg;
	void (*init)(void);
	void (*prepare)(void);
	void (*irq)(void);
	void (*alarm)(void);
	void (*open)(void);
	void (*close)(void);
	void (*read)(void);
	void (*write)(void);
	void (*ioctl)(void);
	void (*cleanup)(void);
	void (*deinit)(void);
} drvr_t;

/* dma.c */
void dma_xfer(unsigned char chan, unsigned long addr, unsigned short len,
	bool read);

/* driver.c */
void drvr_reg_kernel(unsigned char irq);
void drvr_reg_fs(bool block, unsigned char maj);
void drvr_set_alarm(clock_t ticks, unsigned char type);
void drvr_set_wait_alarm(clock_t ticks, unsigned char type,
	unsigned char (*handle_alarm)(msg_t *msg));
void drvr_do_nothing(void);
void drvr_main(drvr_t *drvr);

/* floppy.c */
void fdc_main(void);

/* keyboard.c */
void kbd_irq(unsigned char scode);

/* memory.c */
void mem_main(void);

/* sound.c */
void dsp_main(void);

/* speaker.c */
void speaker_off(void);
void beep(unsigned short freq, clock_t ticks);

/* terminal.c */
void tty_main(void);

/* video.c */
void set_attr(bool blink, unsigned char bg, unsigned char fg);
void clear(void);
int putchar(int c);
int printf(char *format, ...);

#endif
