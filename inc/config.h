/*----------------------------------------------------------------------------*\
 |	config.h							      |
 |									      |
 |	Copyright © 2002-2007, Team Brainix, original authors.		      |
 |		All rights reserved.					      |
\*----------------------------------------------------------------------------*/



/*
 | This file contains user-tunable settings for Brainix.  If you don't know what
 | you're doing, leave this file alone.  The defaults are good enough for me, so
 | they're more than good enough for you.
 */



#include <const.h>
#include <stdbool.h>



/*----------------------------------------------------------------------------*\
 |			     Kernel Configuration:			      |
\*----------------------------------------------------------------------------*/

#define NUM_PROCS	32 /* Maximum number of running processes. */
#define HZ		60 /* What's the frequency, Kenneth?       */



/*----------------------------------------------------------------------------*\
 |			   File System Configuration:			      |
\*----------------------------------------------------------------------------*/

/* The device containing the root file system: */
#define ROOT_MAJ	2 /* Major number. */
#define ROOT_MIN	0 /* Minor number. */

/* Size of a cached block. */
#define BLOCK_SIZE	(1 * KB)

/* File system cache/table sizes: */
#define NUM_BLOCKS	 80 /* Size of block cache.        */
#define NUM_INODES	 64 /* Size of inode table.        */
#define NUM_SUPERS	  8 /* Size of superblock table.   */
#define NUM_FILE_PTRS	128 /* Size of file pointer table. */
#define NUM_DRIVERS	  8 /* Size of driver PID table.   */

/* Robustness: */
#define ROBUST		SLOPPY /* Must be SLOPPY, SANE, or PARANOID. */



/*----------------------------------------------------------------------------*\
 |			    Miscellaneous Settings:			      |
\*----------------------------------------------------------------------------*/

#define DEBUG		true

/* Text colors! */
#define SEVERE_BG	BLACK     /* Severe text background.      */
#define SEVERE_FG	RED       /* Severe text foreground.      */
#define IMPORTANT_BG	BLACK     /* Important text background.   */
#define IMPORTANT_FG	YELLOW    /* Important text foreground.   */
#define NORMAL_BG	BLACK     /* Normal text background.      */
#define NORMAL_FG	WHITE     /* Normal text foreground.      */
#define UNIMPORTANT_BG	BLACK     /* Unimportant text background. */
#define UNIMPORTANT_FG	DARK_GRAY /* Unimportant text foreground. */



/*----------------------------------------------------------------------------*\
 |		There are no tunable settings beyond this point!	      |
\*----------------------------------------------------------------------------*/

/* The following preprocessor directives are used to ensure that the user has
 * chosen sane values for the preceeding settings. */

#if (NUM_PROCS < 8)
#error "In <config.h>, NUM_PROCS must be greater than or equal to 8!"
#endif

#if (ROOT_MAJ != 2 || ROOT_MIN != 0)
#error "In <config.h>, ROOT_MAJ must equal 2 and ROOT_MIN must equal 0!"
#endif

#if (ROBUST < SLOPPY || ROBUST > PARANOID)
#error "In <config.h>, ROBUST must equal SLOPPY, SANE, or PARANOID!"
#endif
