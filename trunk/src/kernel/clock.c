/*----------------------------------------------------------------------------*\
 |	clock.c								      |
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

#define BRAINIX
#include <kernel/kernel.h>

#define RTC_PORT_0	0x70
#define RTC_PORT_1	0x71

#define STAT_A_ADDR	10
#define STAT_B_ADDR	11
#define STAT_C_ADDR	12
#define STAT_D_ADDR	13

#define STAT_A_DV_BITS	0x70
#define STAT_A_UIP_BIT	0x80
#define STAT_B_SET_BIT	0x80

/* Global variables: */
time_t cmos[YEAR + 1];
time_t secs_since_epoch;
bool dirty = true;

/* Function prototypes: */
unsigned char cmos_read(unsigned char addr);
void cmos_write(unsigned char value, unsigned char addr);
unsigned char bcd_to_bin(unsigned char bcd);
unsigned char bin_to_bcd(unsigned char bin);
unsigned char last_day(unsigned char mon, unsigned char year);
void calc_secs_since_epoch(void);
void cmos_get_time(void);
void cmos_set_time(void);
void cmos_update_time(time_t pending);
unsigned char cmos_get_info(unsigned char addr);
void cmos_set_info(unsigned char value, unsigned char addr);

/*----------------------------------------------------------------------------*\
 |				  cmos_read()				      |
\*----------------------------------------------------------------------------*/
unsigned char cmos_read(unsigned char addr)
{

/* Read a value from the CMOS. */

	unsigned char value;

	out_byte(addr, RTC_PORT_0);
	io_delay(4);

	value = in_byte(RTC_PORT_1);
	io_delay(4);

	return value;
}

/*----------------------------------------------------------------------------*\
 |				  cmos_write()				      |
\*----------------------------------------------------------------------------*/
void cmos_write(unsigned char value, unsigned char addr)
{

/* Write a value to the CMOS. */

	out_byte(addr, RTC_PORT_0);
	io_delay(4);

	out_byte(value, RTC_PORT_1);
	io_delay(4);
}

/*----------------------------------------------------------------------------*\
 |				  bcd_to_bin()				      |
\*----------------------------------------------------------------------------*/
unsigned char bcd_to_bin(unsigned char bcd)
{

/* Convert a binary coded decimal number to a binary number. */

	unsigned char bin;
	bin = (bcd >> 4) * 10 + (bcd & 0xF);
	return bin;
}

/*----------------------------------------------------------------------------*\
 |				  bin_to_bcd()				      |
\*----------------------------------------------------------------------------*/
unsigned char bin_to_bcd(unsigned char bin)
{

/* Convert a binary number to a binary coded decimal number. */

	unsigned char bcd;
	bcd = (bin / 10 << 4) | (bin % 10);
	return bcd;
}

/*----------------------------------------------------------------------------*\
 |				   last_day()				      |
\*----------------------------------------------------------------------------*/
unsigned char last_day(unsigned char mon, unsigned char year)
{
	if (!(mon == 2 && year % 4 == 0 && year % 400 != 0))
		switch (mon)
		{
			case  1: return 31;
			case  2: return 28;
			case  3: return 31;
			case  4: return 30;
			case  5: return 31;
			case  6: return 30;
			case  7: return 31;
			case  8: return 31;
			case  9: return 30;
			case 10: return 31;
			case 11: return 30;
			case 12: return 31;
		}
	return 29;
}

/*----------------------------------------------------------------------------*\
 |			    calc_secs_since_epoch()			      |
\*----------------------------------------------------------------------------*/
void calc_secs_since_epoch(void)
{
	unsigned char mon;   /* Month.       */
	unsigned short yday; /* Day of year. */
	unsigned char year;  /* Year.        */

	for (mon = 1, yday = -1; mon < cmos[MON]; mon++)
		yday += last_day(mon, cmos[YEAR]);
	yday += cmos[MDAY];
	year = cmos[YEAR] + 100;
	secs_since_epoch = cmos[SEC] +
		cmos[MIN] * SECS_PER_MIN +
		cmos[HOUR] * SECS_PER_HOUR +
		yday * SECS_PER_DAY +
		(year - 70) * SECS_PER_DAY * 365 +
		(year - 69) / 4 * SECS_PER_DAY -
		(year - 1) / 100 * SECS_PER_DAY +
		(year + 299) / 400 * SECS_PER_DAY;
}

/*----------------------------------------------------------------------------*\
 |				cmos_get_time()				      |
\*----------------------------------------------------------------------------*/
void cmos_get_time(void)
{

/* Read the time from the CMOS. */

	unsigned char j;

	if (!dirty)
		/* Oops.  The time is already synchronized. */
		return;

	while ((cmos_read(STAT_A_ADDR) & STAT_A_UIP_BIT) != STAT_A_UIP_BIT)
		;
	while ((cmos_read(STAT_A_ADDR) & STAT_A_UIP_BIT) == STAT_A_UIP_BIT)
		;

	for (j = 0; j <= YEAR; j++)
		cmos[j] = bcd_to_bin(cmos_read(j));

	calc_secs_since_epoch();
	dirty = false;
}

/*----------------------------------------------------------------------------*\
 |				cmos_set_time()				      |
\*----------------------------------------------------------------------------*/
void cmos_set_time(void)
{

/* Write the time to the CMOS. */

	unsigned char j;

	if (!dirty)
		/* Oops.  The time is already synchronized. */
		return;

	cmos_write(cmos_read(STAT_B_ADDR) | STAT_B_SET_BIT, STAT_B_ADDR);
	cmos_write(cmos_read(STAT_A_ADDR) | STAT_A_DV_BITS, STAT_A_ADDR);

	for (j = 0; j <= YEAR; j++)
		cmos_write(bin_to_bcd(cmos[j]), j);

	cmos_write(cmos_read(STAT_B_ADDR) & ~STAT_B_SET_BIT, STAT_B_ADDR);
	cmos_write(cmos_read(STAT_A_ADDR) & ~STAT_A_DV_BITS, STAT_A_ADDR);

	dirty = false;
}

/*----------------------------------------------------------------------------*\
 |			       cmos_update_time()			      |
\*----------------------------------------------------------------------------*/
void cmos_update_time(time_t pending)
{
	/* At this point, pending is in seconds. */
	secs_since_epoch += pending;
	cmos[SEC] += pending % 60;
	pending /= 60;
	if (cmos[SEC] >= 60)
	{
		cmos[SEC] -= 60;
		pending++;
	}

	/* At this point, pending is in minutes. */
	cmos[MIN] += pending % 60;
	pending /= 60;
	if (cmos[MIN] >= 60)
	{
		cmos[MIN] -= 60;
		pending++;
	}

	/* At this point, pending is in hours. */
	cmos[HOUR] += pending % 24;
	pending /= 24;
	if (cmos[HOUR] >= 24)
	{
		cmos[HOUR] -= 24;
		pending++;
	}

	/* At this point, pending is in days. */
	cmos[WDAY] = (cmos[WDAY] - 1 + pending) % 7 + 1;
	while (cmos[MDAY] + pending > last_day(cmos[MON], cmos[YEAR]))
	{
		pending -= last_day(cmos[MON], cmos[YEAR]) - cmos[MDAY] + 1;
		cmos[MDAY] = 1;
		if (++cmos[MON] > 12)
		{
			cmos[MON] = 1;
			cmos[YEAR]++;
		}
	}
	cmos[MDAY] += pending;
}

/*----------------------------------------------------------------------------*\
 |				cmos_get_info()				      |
\*----------------------------------------------------------------------------*/
unsigned char cmos_get_info(unsigned char addr)
{
	unsigned char value;
	value = cmos[addr];
	return value;
}

/*----------------------------------------------------------------------------*\
 |				cmos_set_info()				      |
\*----------------------------------------------------------------------------*/
void cmos_set_info(unsigned char value, unsigned char addr)
{
	cmos[addr] = value;
	calc_secs_since_epoch();
	dirty = true;
}
