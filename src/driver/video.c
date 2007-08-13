/*----------------------------------------------------------------------------*\
 |	video.c								      |
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

#define DUM_DBUG	-1

/* Registers: */
#define CRTC_INDEX_PORT	0x3D4 /* Index register. */
#define CRTC_DATA_PORT	0x3D5 /* Data register.  */

/* Values to write to the index register: */
#define CRTC_CSR_LOW	0x0F /* Prep data reg for low byte of cursor pos.  */
#define CRTC_CSR_HIGH	0x0E /* Prep data reg for high byte of cursor pos. */

/* Screen geometry: */
#define VID_BUF_BASE	0xB8000 /* Video framebuffer base address. */
#define COLS		80      /* Number of columns.              */
#define ROWS		24      /* Number of rows.                 */

/* Build an attribute byte given blink, background, and foreground values: */
#define ATTR(blink, bg, fg)	((blink << 7) | (bg << 4) | fg)

/* Variables: */
unsigned char attr;  /* Attribute byte. */
unsigned char x_pos; /* Cursor column.  */
unsigned char y_pos; /* Cursor row.     */

/* Function prototypes: */
void set_attr(bool blink, unsigned char bg, unsigned char fg);
void update_cursor(void);
void clear(void);
void itoa(char *s, int base, int n);
int putchar(int c);
int printf(char *format, ...);
void debug(int priority, char *message, void *buf);

/*----------------------------------------------------------------------------*\
 |				   set_attr()				      |
\*----------------------------------------------------------------------------*/
void set_attr(bool blink, unsigned char bg, unsigned char fg)
{
	attr = ATTR(blink, bg, fg);
}

/*----------------------------------------------------------------------------*\
 |				update_cursor()				      |
\*----------------------------------------------------------------------------*/
void update_cursor(void)
{
	unsigned short pos = y_pos * COLS + x_pos;

	out_byte(CRTC_CSR_LOW, CRTC_INDEX_PORT);
	out_byte((pos & 0x00FF) >> 0, CRTC_DATA_PORT);
	out_byte(CRTC_CSR_HIGH, CRTC_INDEX_PORT);
	out_byte((pos & 0xFF00) >> 8, CRTC_DATA_PORT);
}

/*----------------------------------------------------------------------------*\
 |				    clear()				      |
\*----------------------------------------------------------------------------*/
void clear(void)
{
	unsigned char *vid = (unsigned char *) VID_BUF_BASE;
	unsigned short j;

	for (j = 0; j < COLS * ROWS * 2; j += 2)
	{
		*(vid + j) = 0;
		*(vid + j + 1) = attr;
	}

	x_pos = y_pos = 0;
	update_cursor();
}

/*----------------------------------------------------------------------------*\
 |				     itoa()				      |
\*----------------------------------------------------------------------------*/
void itoa(char *s, int base, int n)
{
	char *p = s;
	int divisor = 10, remainder;
	char *left, *right, tmp;

	switch (base)
	{
		case 'd':
		case 'i':
			divisor = 10;
			if (n < 0)
			{
				*p++ = '-';
				s++;
				n = -n;
			}
			break;
		case 'o':
			divisor = 8;
			break;
		case 'u':
			divisor = 10;
			break;
		case 'x':
		case 'X':
			divisor = 16;
			break;
	}

	do
		if ((remainder = n % divisor) < 10)
			*p++ = remainder + '0';
		else
			*p++ = remainder + (base == 'x' ? 'a' : 'A') - 10;
	while (n /= divisor);
	*p = 0;

	for (left = s, right = p - 1; left < right; left++, right--)
	{
		tmp = *left;
		*left = *right;
		*right = tmp;
	}
}

/*----------------------------------------------------------------------------*\
 |				   putchar()				      |
\*----------------------------------------------------------------------------*/
int putchar(int c)
{
	unsigned char *vid = (unsigned char *) VID_BUF_BASE;
	int j;

	switch (c)
	{
		case '\a':
			beep(440, 5);
			return c;
		case '\b':
			if (x_pos > 0)
			{
				x_pos--;
				update_cursor();
			}
			return c;
		case '\f':
			putchar('\v');
			return c;
		case '\n':
			putchar('\r');
			putchar('\v');
			return c;
		case '\r':
			x_pos = 0;
			update_cursor();
			return c;
		case '\t':
			if ((x_pos + 1) % 8 == 0)
				x_pos++;
			if (x_pos % 8 != 0)
				x_pos -= x_pos % 8;
			x_pos += 8;
			if (x_pos >= COLS)
			{
				putchar('\n');
				return c;
			}
			update_cursor();
			return c;
		case '\v':
			y_pos++;
			if (y_pos >= ROWS)
			{
				for (j = 0; j < COLS * ROWS * 2; j++)
					*(vid + j) = *(vid + j + (COLS * 2));
				y_pos--;
			}
			update_cursor();
			return c;
		default:
			*(vid + (x_pos + y_pos * COLS) * 2) = c;
			*(vid + (x_pos + y_pos * COLS) * 2 + 1) = attr;
			x_pos++;
			if (x_pos >= COLS)
			{
				putchar('\n');
				return c;
			}
			update_cursor();
			return c;
	}
}

/*----------------------------------------------------------------------------*\
 |				    printf()				      |
\*----------------------------------------------------------------------------*/
int printf(char *format, ...)
{
	int c, n = 0;
	char s[20], **arg = &format + 1, *p;

	while ((c = *format++))
	{
		if (c != '%')
		{
			putchar(c);
			n++;
			continue;
		}
		switch ((c = *format++))
		{
			case 'd':
			case 'i':
			case 'o':
			case 'u':
			case 'x':
			case 'X':
				itoa(s, c, *((int *) arg++));
				p = s;
				while (*p)
				{
					putchar(*p++);
					n++;
				}
				break;
			case 's':
				if (!(p = *arg++))
					p = "(null)";
				while (*p)
				{
					putchar(*p++);
					n++;
				}
				break;
			default:
				putchar(*((int *) arg++));
				n++;
		}
	}
	return n;
}

void debug(int priority, char *message, void *buf)
{
	if(priority < DUM_DBUG && DUM_DBUG != -1)
		printf(message, buf);
}
