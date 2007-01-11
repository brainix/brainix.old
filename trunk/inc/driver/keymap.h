/*----------------------------------------------------------------------------*\
 |	keymap.h							      |
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

/* Scancodes: */
#define CTRL	0x1D /* Ctrl.              */
#define L_SHIFT	0x2A /* Left Shift.        */
#define R_SHIFT	0x36 /* Right Shift.       */
#define ALT	0x38 /* Alt.               */
#define CAPS	0x3A /* Caps Lock.         */
#define F1	0x3B /* F1.                */
#define F2	0x3C /* F2.                */
#define F3	0x3D /* F3.                */
#define F4	0x3E /* F4.                */
#define F5	0x3F /* F5.                */
#define F6	0x40 /* F6.                */
#define F7	0x41 /* F7.                */
#define F8	0x42 /* F8.                */
#define F9	0x43 /* F9.                */
#define F10	0x44 /* F10.               */
#define NUM	0x45 /* Num Lock.          */
#define SCROLL	0x46 /* Scroll Lock.       */
#define HOME	0x47 /* Home.              */
#define PGUP	0x49 /* PgUp.              */
#define END	0x4F /* End.               */
#define PGDN	0x51 /* PgDn.              */
#define INS	0x52 /* Ins.               */
#define DEL	0x53 /* Del.               */
#define F11	0x57 /* F11.               */
#define F12	0x58 /* F12.               */
#define L_WIN	0x5B /* Left Windows key.  */
#define R_WIN	0x5C /* Right Windows key. */
#define MENU	0x5D /* Menu key.          */

#define BREAK	0x80 /* Bit set when a key is released. */

/* Default keymap: */
unsigned char key_map[] =
{
/* 00h-07h */	0,	0,	'1',	'2',	'3',	'4',	'5',	'6',
/* 08h-0Fh */	'7',	'8',	'9',	'0',	'-',	'=',	'\b',	'\t',
/* 10h-17h */	'q',	'w',	'e',	'r',	't',	'y',	'u',	'i',
/* 18h-1Fh */	'o',	'p',	'[',	']',	'\n',	0,	'a',	's',
/* 20h-27h */	'd',	'f',	'g',	'h',	'j',	'k',	'l',	';',
/* 28h-2Fh */	'\'',	'`',	0,	'\\',	'z',	'x',	'c',	'v',
/* 30h-37h */	'b',	'n',	'm',	',',	'.',	'/',	0,	'*',
/* 38h-3Fh */	0,	' ',	0,	0,	0,	0,	0,	0,
/* 40h-47h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 48h-4Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 50h-57h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 58h-5Eh */	0,	0,	0,	0,	0,	0,	0
};

/* Keymap while ctrl pressed: */
unsigned char ctrl_map[] =
{
/* 00h-07h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 08h-0Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 10h-17h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 18h-1Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 20h-27h */	0,	0,	'\a',	0,	0,	0,	0,	0,
/* 28h-2Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 30h-37h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 38h-3Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 40h-47h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 48h-4Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 50h-57h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 58h-5Eh */	0,	0,	0,	0,	0,	0,	0
};

/* Keymap while shift pressed: */
unsigned char shift_map[] =
{
/* 00h-07h */	0,	0,	'!',	'@',	'#',	'$',	'%',	'^',
/* 08h-0Fh */	'&',	'*',	'(',	')',	'_',	'+',	0,	0,
/* 10h-17h */	'Q',	'W',	'E',	'R',	'T',	'Y',	'U',	'I',
/* 18h-1Fh */	'O',	'P',	'{',	'}',	0,	0,	'A',	'S',
/* 20h-27h */	'D',	'F',	'G',	'H',	'J',	'K',	'L',	':',
/* 28h-2Fh */	'"',	'~',	0,	'|',	'Z',	'X',	'C',	'V',
/* 30h-37h */	'B',	'N',	'M',	'<',	'>',	'?',	0,	0,
/* 38h-3Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 40h-47h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 48h-4Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 50h-57h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 58h-5Eh */	0,	0,	0,	0,	0,	0,	0
};

/* Keymap while alt pressed: */
unsigned char alt_map[] =
{
/* 00h-07h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 08h-0Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 10h-17h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 18h-1Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 20h-27h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 28h-2Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 30h-37h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 38h-3Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 40h-47h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 48h-4Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 50h-57h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 58h-5Eh */	0,	0,	0,	0,	0,	0,	0
};

/* Keymap while caps lock on: */
unsigned char caps_map[] =
{
/* 00h-07h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 08h-0Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 10h-17h */	'Q',	'W',	'E',	'R',	'T',	'Y',	'U',	'I',
/* 18h-1Fh */	'O',	'P',	0,	0,	0,	0,	'A',	'S',
/* 20h-27h */	'D',	'F',	'G',	'H',	'J',	'K',	'L',	0,
/* 28h-2Fh */	0,	0,	0,	0,	'Z',	'X',	'C',	'V',
/* 30h-37h */	'B',	'N',	'M',	0,	0,	0,	0,	0,
/* 38h-3Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 40h-47h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 48h-4Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 50h-57h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 58h-5Eh */	0,	0,	0,	0,	0,	0,	0
};

/* Keymap while num lock on: */
unsigned char num_map[] =
{
/* 00h-07h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 08h-0Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 10h-17h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 18h-1Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 20h-27h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 28h-2Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 30h-37h */	0,	0,	0,	0,	0,	0,	0,	0,
/* 38h-3Fh */	0,	0,	0,	0,	0,	0,	0,	0,
/* 40h-47h */	0,	0,	0,	0,	0,	0,	0,	'7',
/* 48h-4Fh */	'8',	'9',	'-',	'4',	'5',	'6',	'+',	'1',
/* 50h-57h */	'2',	'3',	'0',	'.',	0,	0,	0,	0,
/* 58h-5Eh */	0,	0,	0,	0,	0,	0,	0
};
