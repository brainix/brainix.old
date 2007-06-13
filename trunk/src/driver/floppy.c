/*----------------------------------------------------------------------------*\
 |	floppy.c - 82077AA FDC (Floppy Disk Controller) code		      |
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

#include <driver/driver.h>

/* When debugging the floppy only, set this to be a high number like 10 */
#define	FLOPPY_ESOTERIC	0

/* Major number: */
#define FDC_MAJ		2

/* Alarm types: */
#define ALARM_TIMEOUT	0 /* Stop waiting for an IRQ.               */
#define ALARM_MOTOR	1 /* Turn the motor off.                    */
#define ALARM_SPIN_UP	2 /* Stop waiting for the motor to spin-up. */
#define ALARM_SETTLE	3 /* Stop waiting for the head to settle.   */

/* How long to wait... */
clock_t fdc_ticks[] =
{
/* ALARM_TIMEOUT */  2 * HZ, /* ...for an IRQ.                   (2.000 sec) */
/* ALARM_MOTOR   */  3 * HZ, /* ...before turning the motor off. (3.000 sec) */
/* ALARM_SPIN_UP */  HZ / 4, /* ...for the motor to spin-up.     (0.250 sec) */
/* ALARM_SETTLE  */  HZ / 50 /* ...for the head to settle.       (0.015 sec) */
};

/* Status, data, and control registers: */
#define FDC_SRA_PORT	0x3F0 /* Status register A.              (R  ) */
#define FDC_SRB_PORT	0x3F1 /* Status register B.              (R  ) */
#define FDC_DOR_PORT	0x3F2 /* Digital output register.        (R/W) */
#define FDC_TDR_PORT	0x3F3 /* Tape drive register.            (R/W) */
#define FDC_MSR_PORT	0x3F4 /* Main status register.           (R  ) */
#define FDC_DSR_PORT	0x3F4 /* Data rate select register.      (  W) */
#define FDC_DATA_PORT	0x3F5 /* Data register.                  (R/W) */
#define FDC_DIR_PORT	0x3F7 /* Digital input register.         (R  ) */
#define FDC_CCR_PORT	0x3F7 /* Configuration control register. (  W) */

/* Commands: */
#define FDC_CMD_READ	0xE6 /* Read data.              */
#define FDC_CMD_WRITE	0xC5 /* Write data.             */
#define FDC_CMD_RECAL	0x07 /* Recalibrate.            */
#define FDC_CMD_SENSE	0x08 /* Sense interrupt status. */
#define FDC_CMD_SPEC	0x03 /* Specify.                */
#define FDC_CMD_SEEK	0x0F /* Seek.                   */

/* 3.5" 1.44 MB disk geometry: */
#define GEOM_HEADS	2    /* Heads.                   */
#define GEOM_TRACKS	80   /* Tracks.                  */
#define GEOM_SPT	18   /* Sectors per track.       */
#define GEOM_GAP_3_FMT	0x54 /* Gap length (format).     */
#define GEOM_GAP_3_RW	0x1B /* Gap length (read/write). */

/* True iff the status is ready and the FIFO direction is inward: */
#define FDC_SEND_READY(fdc_msr)	((fdc_msr & 0xC0) == 0x80)

/* True iff the status is ready and the FIFO direction is outward: */
#define FDC_GET_READY(fdc_msr)	((fdc_msr & 0xD0) == 0xD0)

/* True iff a seek or recalibrate completed: */
#define FDC_SEEK_END(fdc_st0)	((fdc_st0 & 0x20) == 0x20)

/* True iff a command terminated normally: */
#define FDC_NORM_TERM(fdc_st0)	((fdc_st0 & 0xC0) == 0x00)

/* Parameters: */
unsigned char fdc_st0; /* Status register 0.       */
unsigned char fdc_st1; /* Status register 1.       */
unsigned char fdc_st2; /* Status register 2.       */
unsigned char fdc_c;   /* Cylinder address.        */
unsigned char fdc_h;   /* Head address.            */
unsigned char fdc_r;   /* Sector address.          */
unsigned char fdc_n;   /* Sector size code.        */
unsigned char fdc_pcn; /* Present cylinder number. */

/* Flags: */
bool fdc_behaving;       /* True while the FDC is behaving.            */
bool fdc_motor_required; /* True while the motor is required to be on. */
bool fdc_motor_on;       /* True while the motor is on.                */

/* DMA: */
unsigned char dma_buf[512 * 2]; /* DMA buffer.              */
unsigned long dma_buf_base;     /* DMA buffer base address. */

/* Global variable: */
msg_t *msg;

/* Function prototypes: */
unsigned char fdc_handle_alarm(msg_t *m);
bool fdc_wait_irq(void);
void fdc_start_motor(void);
void fdc_stop_motor(void);
void fdc_sched_motor(void);
void fdc_send_byte(unsigned char cmd_param);
unsigned char fdc_get_byte(void);
void fdc_reset(void);
void fdc_punish(void);
void fdc_geom(blkcnt_t block, char *head, char *track, char *sector);
void fdc_recal_seek(unsigned char fdc_ncn);
bool fdc_rw_try(blkcnt_t block, bool read);
bool fdc_rw_block(blkcnt_t block, bool read);
void fdc_alarm(void);
void fdc_open_close(void);
void fdc_read_write(void);
void fdc_ioctl(void);
void fdc_main(void);

/*----------------------------------------------------------------------------*\
 |			       fdc_handle_alarm()			      |
\*----------------------------------------------------------------------------*/
unsigned char fdc_handle_alarm(msg_t *m)
{
	debug(-FLOPPY_ESOTERIC+1, "floppy.fdc_handle_alarm(): An alarm has been sounded, performing any necessary actions. Will return the cause of the alarm.\n");
/* An alarm has sounded.  Perform any necessary action.  Return the alarm's
 * cause. */

	debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_handle_alarm(): Perform any necessary action.\n");
	/* Perform any necessary action. */
	switch (m->args.brnx_watch.type)
	{
		case ALARM_TIMEOUT :                   break;
		case ALARM_MOTOR   : fdc_stop_motor(); break;
		case ALARM_SPIN_UP :                   break;
		case ALARM_SETTLE  :                   break;
	}

	debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_handle_alarm(): return the alarm's cause.\n");
	/* Return the alarm's cause. */
	return m->args.brnx_watch.type;
}

/*----------------------------------------------------------------------------*\
 |				 fdc_wait_irq()				      |
\*----------------------------------------------------------------------------*/
bool fdc_wait_irq(void)
{
	debug(-FLOPPY_ESOTERIC+1, "floppy.fdc_wait_irq(): Wait for the FDC to generate an IRQ or time out trying.\n");
/* Wait for the FDC to generate an IRQ, or time out, whichever happens first.
 * Return true on IRQ; false on timeout. */

	msg_t *m;
	bool irq = true;

	if (!fdc_behaving)
	{
		debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_wait_irq(): The FDC is not responding. Abort.\n");
		/* The FDC is not responding.  Abort. */
		return false;
	}
	debug(-FLOPPY_ESOTERIC+7, "floppy.fdc_wait_irq(): Setting the timeout alarm.\n");
	/* Set the timeout alarm. */
	drvr_set_alarm(fdc_ticks[ALARM_TIMEOUT], ALARM_TIMEOUT);

	debug(-FLOPPY_ESOTERIC+7, "floppy.fdc_wait_irq(): Wait for the FDC to generate an IRQ or timeout.\n");
	/* Wait for the FDC to generate an IRQ or time out. */
	while (true)
	{
		if ((m = msg_check(HARDWARE)) != NULL)
		{
			debug(-FLOPPY_ESOTERIC+1,"floppy.fdc_wait_irq(): The FDC has generated an IRQ.\n");
			/* The FDC has generated an IRQ.  Stop waiting. */
			break;
		}
		if ((m = msg_check(TMR_PID)) != NULL)
		{
			if (fdc_handle_alarm(m) == ALARM_TIMEOUT)
			{
				debug(-FLOPPY_ESOTERIC+3,"floppy.fdc_wait_irq(): The FDC has generated an IRQ.\n");
				/* The FDC has timed out.  Stop waiting. */
				irq = false;
				break;
			}
			else
			{
				/* A previous alarm has sounded - not the
				 * timeout alarm.  Keep waiting. */
				debug(-FLOPPY_ESOTERIC+3,"floppy.fdc_wait_irq(): A previous alarm has sounded - not the timeout alarm...keep waiting.\n");
				msg_free(m);
				continue;
			}
		}
		debug(-FLOPPY_ESOTERIC+2,"floppy.fdc_wait_irq(): The FDC has not yet generated an IRQ or timed out. Keep waiting.\n");
		/* The FDC has not yet generated an IRQ or timed out.  Keep
		 * waiting. */
		proc_nap();
	}
	msg_free(m);

	/* Return true on IRQ; false on timeout. */
	return irq;
}

/*----------------------------------------------------------------------------*\
 |			       fdc_start_motor()			      |
\*----------------------------------------------------------------------------*/
void fdc_start_motor(void)
{

/* Turn on the floppy motor and wait for it to spin-up.  This function is called
 * before each floppy access. */
	debug(-FLOPPY_ESOTERIC+7, "floppy.fdc_start_motor(): The floppy is now being accessed.\n");
	/* The floppy is now being accessed. */
	fdc_motor_required = true;

	if (fdc_motor_on)
	{
		debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_start_motor(): The motor is already on and spinning at speed.\n");
		/* The motor is already on and spinning at speed. */
		return;
	}

	debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_start_motor(): Turn on the motor.\n");
	/* Turn on the motor. */
	out_byte(0x1C, FDC_DOR_PORT);

	debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_start_motor(): Wait for the motor to spin up.\n");
	/* Wait for the motor to spin-up. */
	drvr_set_wait_alarm(fdc_ticks[ALARM_SPIN_UP], ALARM_SPIN_UP,
		fdc_handle_alarm);

	debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_start_motor(): The motor is now on and spinning at speed.\n");
	/* The motor is now on and spinning at speed. */
	fdc_motor_on = true;
}

/*----------------------------------------------------------------------------*\
 |				fdc_stop_motor()			      |
\*----------------------------------------------------------------------------*/
void fdc_stop_motor(void)
{

/* Turn off the motor.  This function is called only by the alarm handler. */

	if (fdc_motor_required)
	{
		debug(-FLOPPY_ESOTERIC+1, "floppy.fdc_stop_motor(): The floppy is currently being accessed. The motor cannot be turned pff.\n");
		/* The floppy is currently being accessed.  The motor cannot be
		 * turned off. */
		return;
	}

	debug(-FLOPPY_ESOTERIC+1, "floppy.fdc_stop_motor(): Turn off the motor.\n"); 
	/* Turn off the motor. */
	out_byte(0x0C, FDC_DOR_PORT);

	debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_stop_motor(): The motor is now off.\n");
	/* The motor is now off. */
	fdc_motor_on = false;
}

/*----------------------------------------------------------------------------*\
 |			       fdc_sched_motor()			      |
\*----------------------------------------------------------------------------*/
void fdc_sched_motor(void)
{

/*
 | Schedule the motor to be turned off in 3 seconds.  This function is called
 | after each floppy access.  If a new floppy access begins after the last one
 | has completed and before the 3 seconds have passed, the motor is left on and
 | is rescheduled to be turned off 3 seconds after the new access has completed.
 */
	debug(-FLOPPY_ESOTERIC+1, "floppy.fdc_sched_motor(): The floppy is no longer being accessed.\n");
	/* The floppy is no longer being accessed. */
	fdc_motor_required = false;

	debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_sched_motor(): Schedule the motor to be turned off.\n");
	/* Schedule the motor to be turned off. */
	drvr_set_alarm(fdc_ticks[ALARM_MOTOR], ALARM_MOTOR);
}

/*----------------------------------------------------------------------------*\
 |				fdc_send_byte()				      |
\*----------------------------------------------------------------------------*/
void fdc_send_byte(unsigned char cmd_param)
{

/* Wait until the FDC is ready, then send a command or parameter byte. */

	unsigned char fdc_msr;
	unsigned char timeout = 128;

	if (!fdc_behaving)
	{
		debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_send_byte(): The FDC is not responding. Abort!\n");
		/* The FDC is not responding.  Abort. */
		return;
	}

	debug(-FLOPPY_ESOTERIC+1, "floppy.fdc_send_byte(): Wait until the FDC is ready.\n");
	/* Wait until the FDC is ready. */
	while (!FDC_SEND_READY(fdc_msr = in_byte(FDC_MSR_PORT)) && --timeout)
		;

	if (FDC_SEND_READY(fdc_msr))
	{
		debug(-FLOPPY_ESOTERIC+3, "floppy.fdc_send_byte(): The FDC is ready. Send the Command or parameter byte.\n"); 
		/* The FDC is ready.  Send the command or parameter byte. */
		out_byte(cmd_param, FDC_DATA_PORT);
	}
	else
	{
		debug(-FLOPPY_ESOTERIC+1, "floppy.fdc_send_byte(): The FDC is not responding, give it a good spank.\n");
		/* The FDC is not responding.  Give it a spanking. */
		fdc_behaving = false;
	}
}

/*----------------------------------------------------------------------------*\
 |				 fdc_get_byte()				      |
\*----------------------------------------------------------------------------*/
unsigned char fdc_get_byte(void)
{

/* Wait until the FDC is ready, then read a result byte. */

	unsigned char fdc_msr;
	unsigned char timeout = 128;
	unsigned char result = 0;

	if (!fdc_behaving)
	{
		debug(-FLOPPY_ESOTERIC+2,"floppy.fdc_get_byte(): FDC is not responding...abort.\n");

		/* The FDC is not responding.  Abort. */
		return result;
	}

	/* Wait until the FDC is ready. */
	debug(-FLOPPY_ESOTERIC+2,"floppy.fdc_get_byte(): Waiting for the FDC to be ready.\n");

	while (!FDC_GET_READY(fdc_msr = in_byte(FDC_MSR_PORT)) && --timeout)
		;

	if (FDC_GET_READY(fdc_msr))
	{

		debug(-FLOPPY_ESOTERIC+2,"floppy.fdc_get_byte(): The FDC is ready, reading the result byte.\n");
		/* The FDC is ready.  Read the result byte. */
		result = in_byte(FDC_DATA_PORT);
	}
	else
	{
		debug(-FLOPPY_ESOTERIC+2,"floppy.fdc_get_byte(): The FDC is not responding, preparing spank.\n");
		/* The FDC is not responding.  Give it a spanking. */
		fdc_behaving = false;
	}
	return result;
}

/*----------------------------------------------------------------------------*\
 |				  fdc_reset()				      |
\*----------------------------------------------------------------------------*/
void fdc_reset(void)
{

/* Reset the FDC. */

	unsigned char j;
 
	debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_reset(): Causing a reset, programming the data rate, and waiting for an IRQ.\n");
	/* Cause a reset, program the data rate, and wait for an IRQ. */
	out_byte(0x00, FDC_DOR_PORT); /* Strobe the DOR's reset bits low. */
	out_byte(0x0C, FDC_DOR_PORT); /* Strobe them high again to reset. */
	out_byte(0x00, FDC_CCR_PORT); /* Program the data rate.           */
	fdc_wait_irq();               /* Wait for an IRQ.                 */

	/* For the 1st drive, sense the interrupt status and save the results.
	 * For the 2nd through 4th drives, sense the interrupt statuses and
	 * discard the results.  Brainix only supports one floppy drive, but
	 * this must be done anyway as part of the FDC (re)initialization. */
	for (j = 0; j < 4; j++)
	{
		fdc_send_byte(FDC_CMD_SENSE); /* Send sense intr status cmd. */
		if (j == 0)
		{
			fdc_st0 = fdc_get_byte(); /* Read status register 0.  */
			fdc_pcn = fdc_get_byte(); /* Read present cyl number. */
			continue;
		}
		fdc_get_byte(); /* Discard status register 0.       */
		fdc_get_byte(); /* Discard present cylinder number. */
	}

	/* Set the initial values for the FDC's internal timers. */
	fdc_send_byte(FDC_CMD_SPEC); /* Send specify command.              */
	fdc_send_byte(0xDF);         /* Send step rate, head unload time.  */
	fdc_send_byte(0x02);         /* Send head load time, non-DMA flag. */

	debug(-FLOPPY_ESOTERIC+1, "floppy.fdc_reset(): The FDC is now ready to accept commands.\n");
	/* The FDC is now ready to accept commands. */
}

/*----------------------------------------------------------------------------*\
 |				  fdc_punish()				      |
\*----------------------------------------------------------------------------*/
void fdc_punish(void)
{

/* (Re)initialize the FDC.  This function is called before accessing the FDC,
 * and whenever the FDC is not responding and needs a spanking. */

	static bool first_time = true;

	if (!first_time && fdc_behaving)
		return;

	debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_punish(): (Re)initializing the flags, make sure they're in sync with reality.\n");
	/* (Re)initialize the flags.  Make sure they're in sync with reality. */
	fdc_behaving = true;
	fdc_motor_required = false;
	fdc_motor_on = false;

	if (first_time)
	{
		debug(-FLOPPY_ESOTERIC+9, "floppy.fdc_punish(): Make sure the DMA buffer doesn't cross a 64 KB boundary.\n");
		/* Make sure the DMA buffer doesn't cross a 64 KB boundary. */
		dma_buf_base = (unsigned long) dma_buf;
		if (dma_buf_base % (64 * KB) > 64 * KB - 512)
			dma_buf_base += 512;

		debug(-FLOPPY_ESOTERIC+4, "floppy.fdc_punish(): Register the floppy driver with the kernel.\n");
		/* Register the floppy driver with the kernel. */
		drvr_reg_kernel(FDC_IRQ);
	}

	debug(-FLOPPY_ESOTERIC+2, "floppy.fdc_punish(): Resetting the FDC.\n");
	/* Reset the FDC. */
	fdc_reset();

	if (first_time)
	{
		debug(-FLOPPY_ESOTERIC+3, "floppy.fdc_punish(): Register the floppy driver with the file system.\n");
		/* Register the floppy driver with the file system. */
		drvr_reg_fs(BLOCK, FDC_MAJ);

		/* The DMA buffer base will never need to be re-calculated, and
		 * the floppy driver will never need to be re-registered.  Only
		 * the brain-damaged FDC may require a reset. */
		first_time = false;
	}
}

/*----------------------------------------------------------------------------*\
 |				   fdc_geom()				      |
\*----------------------------------------------------------------------------*/
void fdc_geom(blkcnt_t block, char *head, char *track, char *sector)
{

/* Based on the 3.5" 1.44 MB disk geometry, convert a 512-byte block number to
 * head, track, and sector numbers. */

	*head = (block % (GEOM_SPT * GEOM_HEADS)) / GEOM_SPT;
	*track = block / (GEOM_SPT * GEOM_HEADS);
	*sector = block % GEOM_SPT + 1;
}

/*----------------------------------------------------------------------------*\
 |				fdc_recal_seek()			      |
\*----------------------------------------------------------------------------*/
void fdc_recal_seek(unsigned char fdc_ncn)
{
	debug(-FLOPPY_ESOTERIC+9, "floppy.fdc_recal_seek(): move the disk head to a track.\n");
/* Move the disk head to a track. */

	bool recal = fdc_ncn == 0;
	bool seek = !recal;

	if (seek && fdc_ncn == fdc_pcn)
	{
		debug(-FLOPPY_ESOTERIC+3, "floppy.fdc_recal_seek(): The head is already at the requested track.\n");
		/* The head is already at the requested track. */
		return;
	}

	if (recal)
	{
		debug(-FLOPPY_ESOTERIC+3, "floppy.fdc_recal_seek(): send the recalibrate command and parameter byte.\n");
		/* Send the recalibrate command and parameter byte. */
		fdc_send_byte(FDC_CMD_RECAL); /* Recalibrate command. */
		debug(-FLOPPY_ESOTERIC+5, "floppy.fdc_recal_seek(): Recalibrate command sent.\n");
		fdc_send_byte(0x00);          /* Disk drive select.   */
		debug(-FLOPPY_ESOTERIC+5, "floppy.fdc_recal_seek(): Disk drive select. \n");
	}
	if (seek)
	{
		debug(-FLOPPY_ESOTERIC+3, "floppy.fdc_recal_seek(): send the seek command and parameter bytes.\n");
		/* Send the seek command and parameter bytes. */
		debug(-FLOPPY_ESOTERIC+5, "floppy.fdc_recal_seek(): Seek command sent.\n");
		fdc_send_byte(FDC_CMD_SEEK); /* Seek command.                 */
		debug(-FLOPPY_ESOTERIC+5, "floppy.fdc_recal_seek(): Head address, disk drive selected.\n");
		fdc_send_byte(0x00);         /* Head addr, disk drive select. */
		debug(-FLOPPY_ESOTERIC+5, "floppy.fdc_recal_seek(): New Cylinder number selected.\n");
		fdc_send_byte(fdc_ncn);      /* New cylinder number.          */
	}

	fdc_wait_irq();               /* Wait for an IRQ.                     */
	fdc_send_byte(FDC_CMD_SENSE); /* Send sense interrupt status command. */
	fdc_st0 = fdc_get_byte();     /* Read status register 0.              */
	fdc_pcn = fdc_get_byte();     /* Read present cylinder number.        */

	/*
	 | Wait for the head to settle.  This is overkill - sending a message,
	 | awaiting a reply - just for a 15 ms delay.  But I prefer a complex
	 | solution to a simple Non-Solution ®.
	 |
	 | Non-Solution is a registered trademark of Microsoft Corporation in
	 | the United States and other countries.
	 */
	drvr_set_wait_alarm(fdc_ticks[ALARM_SETTLE], ALARM_SETTLE,
		fdc_handle_alarm);

	if (!FDC_SEEK_END(fdc_st0) || (seek && fdc_ncn != fdc_pcn))
	{
		debug(-FLOPPY_ESOTERIC+1, "floppy.fdc_recal_seek(): The FDC is not responding, give it a good spank.\n");
		/* The FDC is not responding.  Give it a spanking. */
		fdc_behaving = false;
	}
}

/*----------------------------------------------------------------------------*\
 |				  fdc_rw_try()				      |
\*----------------------------------------------------------------------------*/
bool fdc_rw_try(blkcnt_t block, bool read)
{

/*
 | If read is true, try once to read a 512-byte block from the floppy disk into
 | the DMA buffer.  Otherwise, try once to write a 512-byte block from the DMA
 | buffer to the floppy disk.  Return whether the read/write should NOT be tried
 | again.
 |
 | There are two cases in which the read/write should NOT be tried again:
 |	1. The FDC stops responding.
 |	2. The read/write succeeds.
 */

	unsigned char cmd = read ? FDC_CMD_READ : FDC_CMD_WRITE;
	char head, track, sector;
	fdc_geom(block, &head, &track, &sector);

	debug(-FLOPPY_ESOTERIC+1, "floppy.rw_try(): Initialize the DMA controller.\n");
	/* Initialize the DMA controller. */
	dma_xfer(FDC_DMA, dma_buf_base, 512, read);

	debug(-FLOPPY_ESOTERIC+2, "floppy.rw_try(): Send the read/write command and parameter bytes.\n");
	/* Send the read or write command and parameter bytes. */
	fdc_send_byte(cmd);           /* Read or write command.           */
	fdc_send_byte(head << 2);     /* Head address, disk drive select. */
	fdc_send_byte(track);         /* Cylinder address.                */
	fdc_send_byte(head);          /* Head address.                    */
	fdc_send_byte(sector);        /* Sector address.                  */
	fdc_send_byte(0x02);          /* Sector size code.                */
	fdc_send_byte(GEOM_SPT);      /* End of track.                    */
	fdc_send_byte(GEOM_GAP_3_RW); /* Gap length.                      */
	fdc_send_byte(0xFF);          /* Special sector size.             */

	debug(-FLOPPY_ESOTERIC+1, "floppy.rw_try(): Wait for Irq.\n");
	/* Wait for an IRQ. */
	if (!fdc_wait_irq())
	{
		debug(-FLOPPY_ESOTERIC+3, "floppy.rw_try(): The FDC is not responding so give it a good spanking.\n");
		/* The FDC is not responding.  Give it a spanking. */
		return fdc_behaving = false;
	}

	debug(-FLOPPY_ESOTERIC+1, "floppy.rw_try(): Read the result bytes.\n");
	/* Read the result bytes. */
	fdc_st0 = fdc_get_byte(); /* Status register 0. */
	fdc_st1 = fdc_get_byte(); /* Status register 1. */
	fdc_st2 = fdc_get_byte(); /* Status register 2. */
	fdc_c = fdc_get_byte();   /* Cylinder address.  */
	fdc_h = fdc_get_byte();   /* Head address.      */
	fdc_r = fdc_get_byte();   /* Sector address.    */
	fdc_n = fdc_get_byte();   /* Sector size code.  */

	debug(-FLOPPY_ESOTERIC+1, "floppy.rw_try(): Check the status.\n");
	/* Check the status. */
	return FDC_NORM_TERM(fdc_st0);
}

/*----------------------------------------------------------------------------*\
 |				 fdc_rw_block()				      |
\*----------------------------------------------------------------------------*/
bool fdc_rw_block(blkcnt_t block, bool read)
{
	debug(-FLOPPY_ESOTERIC+1,"floppy.fdc_rw_block(): Reading/Writing a 512 byte block to/from the DMA buffer.\n");
/* If read is true, read a 512-byte block from the floppy disk into the DMA
 * buffer.  Otherwise, write a 512-byte block from the DMA buffer to the floppy
 * disk.  Return whether the FDC is behaving. */

	char seek_try, rw_try;
	char head, track, sector;
	fdc_geom(block, &head, &track, &sector);

	debug(-FLOPPY_ESOTERIC+1,"floppy.fdc_rw_block(): Prepare for the read/write.\n");
	/* Prepare for the read/write. */
	fdc_start_motor();
	out_byte(0x00, FDC_CCR_PORT);

	debug(-FLOPPY_ESOTERIC+1,"floppy.fdc_rw_block(): Attempt to position the head over the track 3 times.\n");
	/* Attempt to position the head over the track 3 times, or until the
	 * read/write has succeeded, whichever happens first. */
	for (seek_try = 0; seek_try < 3; seek_try++)
	{
		fdc_recal_seek(0);
		fdc_recal_seek(track);

		debug(-FLOPPY_ESOTERIC+2,"floppy.fdc_rw_block(): Attempt to read/write 3 times.\n");
		/* Attempt the read/write 3 times, or until it has succeeded,
		 * whichever happens first. */
		for (rw_try = 0; rw_try < 3; rw_try++)
			if (fdc_rw_try(block, read))
			{
				debug(-FLOPPY_ESOTERIC+3,"floppy.fdc_rw_block(): Read/write is a success!\n");
				/* The read/write has succeeded! */
				fdc_sched_motor();
				return fdc_behaving;
			}

		debug(-FLOPPY_ESOTERIC+2,"floppy.fdc_rw_block(): Read/write has failed 3 times.\n");
		/* The read/write has failed 3 times.  This persisting failure
		 * could be due to improper alignment between the head and the 
		 * track.  Reposition the head and try the read/write again. */
		fdc_recal_seek(0);
	}

	debug(-FLOPPY_ESOTERIC+1,"floppy.fdc_rw_block(): failed too many times, time for spanking.\n");
	/* The FDC is not responding.  Give it a spanking. */
	fdc_sched_motor();
	return fdc_behaving = false;
}

/*----------------------------------------------------------------------------*\
 |				  fdc_alarm()				      |
\*----------------------------------------------------------------------------*/
void fdc_alarm(void)
{
	debug(-FLOPPY_ESOTERIC+1, "floppy.fdc_alarm(): fdc_alarm called.\n");
	fdc_handle_alarm(msg);
}

/*----------------------------------------------------------------------------*\
 |				fdc_open_close()			      |
\*----------------------------------------------------------------------------*/
void fdc_open_close(void)
{
	debug(-FLOPPY_ESOTERIC+5,"floppy.fdc_open_close(): Attempting to open or close the floppy.\n");
	if (msg->args.open_close.min != 0)
	{
		debug(-FLOPPY_ESOTERIC+2,"floppy.fdc_open_close(): Nothing happened, the return values was set to %d.\n",-ENXIO);
		msg->args.open_close.ret_val = -ENXIO;
	}
	else
	{

		debug(-FLOPPY_ESOTERIC+5,"floppy.fdc_open_close(): Nothing happened, the return value was just set to 0.");
		msg->args.open_close.ret_val = 0;
	}
}

/*----------------------------------------------------------------------------*\
 |				fdc_read_write()			      |
\*----------------------------------------------------------------------------*/
void fdc_read_write(void)
{
	debug(-FLOPPY_ESOTERIC+1,"fdc_read_write(): reading or writing to/from the floppy.\n");

	bool read = msg->op == SYS_READ;
	char *p1 = read ? msg->args.read_write.buf : (char *) dma_buf_base;
	char *p2 = read ? (char *) dma_buf_base : msg->args.read_write.buf;
	int p1_addend, p2_addend;
	int block = msg->args.read_write.off / 512;
	int index = msg->args.read_write.off % 512;
	size_t remaining = msg->args.read_write.size, completed = 0, size;

	if (msg->args.read_write.min != 0)
	{
		msg->args.read_write.ret_val = -ENXIO;

		debug(-FLOPPY_ESOTERIC+2,"floppy.read_write(): ERROR IN READING/WRITING IN/OUT.\n");

		return;
	}

	while (remaining)
	{
		debug(-FLOPPY_ESOTERIC+2,"fdc_read_write(): %d bytes remaining.\n", remaining);

		size = remaining < 512 ? remaining : 512;
		if (read || index != 0 || size != 512)
			if (!fdc_rw_block(block, READ))
			{
				debug(-FLOPPY_ESOTERIC+3,"fdc_read_write(): while loop broken method 1.\n");
				break;
			}
		p1_addend = read ? completed : index;
		p2_addend = read ? index : completed;
		memcpy(p1 + p1_addend, p2 + p2_addend, size);
		if (!read)
			if (!fdc_rw_block(block, WRITE))
			{
				debug(-FLOPPY_ESOTERIC+4,"fdc_read_write(): while loop broken method 2.\n");
				break;
			}
		index = 0;
		remaining -= size;
		completed += size;
		block++;
	}
	if(fdc_behaving)
		debug(-FLOPPY_ESOTERIC+2,"fdc_read_write(): behaving...good boy.\n");
	else
		debug(-FLOPPY_ESOTERIC+2,"fdc_read_write(): Things have gone awry here.\n");
	msg->args.read_write.ret_val = fdc_behaving ? completed : -EIO;
}

/*----------------------------------------------------------------------------*\
 |				  fdc_ioctl()				      |
\*----------------------------------------------------------------------------*/
void fdc_ioctl(void)
{
	if (msg->args.open_close.min != 0)
		msg->args.open_close.ret_val = -ENXIO;
	else
		msg->args.open_close.ret_val = -ENOTTY;
}

/*----------------------------------------------------------------------------*\
 |				   fdc_main()				      |
\*----------------------------------------------------------------------------*/
void fdc_main(void)
{
	drvr_t drvr =
	{
		&msg,
		fdc_punish,      /* init()    */
		drvr_do_nothing, /* prepare() */
		drvr_do_nothing, /* irq()     */
		fdc_alarm,       /* alarm()   */
		fdc_open_close,  /* open()    */
		fdc_open_close,  /* close()   */
		fdc_read_write,  /* read()    */
		fdc_read_write,  /* write()   */
		fdc_ioctl,       /* ioctl()   */
		fdc_punish,      /* cleanup() */
		fdc_stop_motor   /* deinit()  */
	};

	drvr_main(&drvr);
}
