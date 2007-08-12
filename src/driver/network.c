/*----------------------------------------------------------------------------*\
 |	network.c							      |
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

#define COMMAND			0x300
#define PAGE_START		(COMMAND + 0x01)
#define PAGE_STOP		(COMMAND + 0x02)
#define BOUNDARY		(COMMAND + 0x03)
#define TRANSMIT_STATUS		(COMMAND + 0x04)
#define TRANSMIT_PAGE		(COMMAND + 0x04)
#define TRANSMIT_BYTE_COUNT_0	(COMMAND + 0x05)
#define NCR			(COMMAND + 0x05)
#define TRANSMIT_BYTE_COUNT_1	(COMMAND + 0x06)
#define INTERRUPT_STATUS	(COMMAND + 0x07)
#define CURRENT			(COMMAND + 0x07) /* In page 1. */
#define REMOTE_START_ADDRESS_0	(COMMAND + 0x08)
#define CR_DMA_0		(COMMAND + 0x08)
#define REMOTE_START_ADDRESS_1	(COMMAND + 0x09)
#define CR_DMA_1		(COMMAND + 0x09)
#define REMOTE_BYTE_COUNT_0	(COMMAND + 0x0A)
#define REMOTE_BYTE_COUNT_1	(COMMAND + 0x0B)
#define RECEIVE_STATUS		(COMMAND + 0x0C)
#define RECEIVE_CONFIGURATION	(COMMAND + 0x0C)
#define TRANSMIT_CONFIGURATION	(COMMAND + 0x0D)
#define FAE_TALLY		(COMMAND + 0x0D)
#define DATA_CONFIGURATION	(COMMAND + 0x0E)
#define CRC_TALLY		(COMMAND + 0x0E)
#define INTERRUPT_MASK		(COMMAND + 0x0F)
#define MISS_PKT_TALLY		(COMMAND + 0x0F)
#define IO_PORT			(COMMAND + 0x10)

#define PSTART			0x46
#define PSTOP			0x80
#define TRANSMIT_BUFFER		0x40

/* Global variables: */
unsigned char rcr = 0x00; /* Value for receive configuration register.  */
unsigned char tcr = 0x00; /* Value for transmit configuration register. */
unsigned char dcr = 0x58; /* Value for data configuration register.     */
unsigned char imr = 0x0B; /* Value for interrupt mask register.         */

/* Function prototypes: */
void net_init(void);
void net_send(void);
void net_queue(void);
void pc_to_net(unsigned short pkt, unsigned short pg, unsigned char n);
void net_to_pc(unsigned short pkt, unsigned short pg, unsigned char n);
void net_xfer(bool dir, unsigned short pg, unsigned short pkt, unsigned char n);
void net_irq(void);
void net_main(void);

/*----------------------------------------------------------------------------*\
 |				   net_init()				      |
\*----------------------------------------------------------------------------*/
void net_init(void)
{

/* Initialize the NIC. */

	out_byte(0x21, COMMAND);
	out_byte(dcr,  DATA_CONFIGURATION);
	out_byte(0x00, REMOTE_BYTE_COUNT_0);
	out_byte(0x00, REMOTE_BYTE_COUNT_1);
	out_byte(rcr,  RECEIVE_CONFIGURATION);
	out_byte(0x20, TRANSMIT_PAGE);
	out_byte(0x02, TRANSMIT_CONFIGURATION);
	out_byte(0x26, PAGE_START);
	out_byte(0x26, BOUNDARY);
	out_byte(0x40, PAGE_STOP);
	out_byte(0x61, COMMAND);
	out_byte(0x26, CURRENT);
	out_byte(0x22, COMMAND);
	out_byte(0xFF, INTERRUPT_STATUS);
	out_byte(imr,  INTERRUPT_MASK);
	out_byte(tcr,  TRANSMIT_CONFIGURATION);
}

/*----------------------------------------------------------------------------*\
 |				   net_send()				      |
\*----------------------------------------------------------------------------*/
void net_send(void)
{

/* If the transmitter is busy, queue a packet.  Otherwise, transmit the
 * packet. */

	/* Is the transmitter busy? */
	if (in_byte(COMMAND) == 0x26)
		/* Yes - queue the packet. */
		net_queue();
	else
	{
		/* No - transmit the packet. */
	}
}

/*----------------------------------------------------------------------------*\
 |				  net_queue()				      |
\*----------------------------------------------------------------------------*/
void net_queue(void)
{
}

/*----------------------------------------------------------------------------*\
 |				  pc_to_net()				      |
\*----------------------------------------------------------------------------*/
void pc_to_net(unsigned short pkt, unsigned short pg, unsigned char n)
{

/* Transfer a packet from the PC's RAM to the NIC's RAM. */

	net_xfer(true, pkt, pg, n);
}

/*----------------------------------------------------------------------------*\
 |				  net_to_pc()				      |
\*----------------------------------------------------------------------------*/
void net_to_pc(unsigned short pkt, unsigned short pg, unsigned char n)
{

/* Transfer a packet from the NIC's RAM to the PC's RAM. */

	net_xfer(false, pkt, pg, n);
}

/*----------------------------------------------------------------------------*\
 |				   net_xfer()				      |
\*----------------------------------------------------------------------------*/
void net_xfer(bool dir, unsigned short pg, unsigned short pkt, unsigned char n)
{

/* If dir(ection) is true, transfer a packet from the PC's RAM to the NIC's RAM.
 * Otherwise, transfer a packet from the NIC's RAM to the PC's RAM. */

	if (n % 2)
		n++;
	out_byte(n & 0xFF, REMOTE_BYTE_COUNT_0);
	out_byte(n >> 16, REMOTE_BYTE_COUNT_1);
	out_byte(pg & 0xFF, REMOTE_START_ADDRESS_0);
	out_byte(pg >> 16, REMOTE_START_ADDRESS_1);
	out_byte(dir ? 0x12 : 0x0A, COMMAND);

	while (in_byte(INTERRUPT_STATUS) != 0x40)
		;
	out_byte(0x40, INTERRUPT_STATUS);
}

/*----------------------------------------------------------------------------*\
 |				   net_irq()				      |
\*----------------------------------------------------------------------------*/
void net_irq(void)
{
}

/*----------------------------------------------------------------------------*\
 |				   net_main()				      |
\*----------------------------------------------------------------------------*/
void net_main(void)
{
}
