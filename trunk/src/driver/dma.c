/*----------------------------------------------------------------------------*\
 |	dma.c - 8237A DMA (Direct Memory Access) controller code	      |
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

#include <driver/driver.h>

/* Registers: */
#define DMA_MASK_PORT	0x0A /* Mask register. */
#define DMA_MODE_PORT	0x0B /* Mode register. */
#define DMA_FLIP_FLOP	0x0C /* Flip-flop.     */

/* Values for registers: */
#define DMA_MASK_BIT	0x04 /* Bit to mask a channel.               */
#define DMA_READ	0x44 /* Single, increment, auto-init, read.  */
#define DMA_WRITE	0x48 /* Single, increment, auto-init, write. */

/* Each channel has its own registers: */
typedef struct
{
	unsigned short pg;  /* Page register.   */
	unsigned short off; /* Offset register. */
	unsigned short len; /* Length register. */
} dma_chan_t;

/* A nice array of the channels' registers: */
dma_chan_t dma_chan[] =
{
      /* Page,  Offset, Length. */
	{0x87,   0x00,   0x01}, /* Channel 0. */
	{0x83,   0x02,   0x03}, /* Channel 1. */
	{0x81,   0x04,   0x05}, /* Channel 2. */
	{0x82,   0x06,   0x07}  /* Channel 3. */
};

/* Function prototype: */
void dma_xfer(unsigned char chan, unsigned long addr, unsigned short len,
	bool read);

/*----------------------------------------------------------------------------*\
 |				   dma_xfer()				      |
\*----------------------------------------------------------------------------*/
void dma_xfer(unsigned char chan, unsigned long addr, unsigned short len,
	bool read)
{

/*
 | Set up a DMA transfer over the specified channel, starting at the specified
 | address, for the specified length.  If read is true, the transfer is from the
 | device to RAM.  Otherwise, the transfer is from RAM to the device.
 */

	unsigned char mode = (read ? DMA_READ : DMA_WRITE) | chan;
	unsigned char   pg = (addr & 0xFF0000) >> 16;
	unsigned short off = (addr & 0x00FFFF) >> 0;
	len--;

	/* Mask the channel, clear the flip-flop, and set the mode. */
	out_byte(chan | DMA_MASK_BIT, DMA_MASK_PORT);
	out_byte(0x00, DMA_FLIP_FLOP);
	out_byte(mode, DMA_MODE_PORT);

	/* Set the starting address and length of the transfer. */
	out_byte(pg, dma_chan[chan].pg);
	out_byte((off & 0x00FF) >> 0, dma_chan[chan].off);
	out_byte((off & 0xFF00) >> 8, dma_chan[chan].off);
	out_byte((len & 0x00FF) >> 0, dma_chan[chan].len);
	out_byte((len & 0xFF00) >> 8, dma_chan[chan].len);

	/* Unmask the channel. */
	out_byte(chan, DMA_MASK_PORT);
}
