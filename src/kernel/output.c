/*----------------------------------------------------------------------------*\
 |	output.c							      |
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

/* Function prototypes: */
void print_version(void);
void print_time(void);
void print_uptime(void);
void print_emoticon(unsigned char emoticon);
void print_op(unsigned char op);
void print_pid(pid_t pid);

/*----------------------------------------------------------------------------*\
 |				print_version()				      |
\*----------------------------------------------------------------------------*/
void print_version(void)
{
	printf("Brainix!\n");
}

/*----------------------------------------------------------------------------*\
 |				  print_time()				      |
\*----------------------------------------------------------------------------*/
void print_time(void)
{
/*
	unsigned char wday = cmos_get_info(WDAY);
	unsigned char  mon = cmos_get_info(MON);
	unsigned char mday = cmos_get_info(MDAY);
	unsigned char hour = cmos_get_info(HOUR);
	unsigned char  min = cmos_get_info(MIN);
	unsigned char  sec = cmos_get_info(SEC);
	unsigned char year = cmos_get_info(YEAR);

	switch (wday)
	{
		case 1: printf("Sun "); break;
		case 2: printf("Mon "); break;
		case 3: printf("Tue "); break;
		case 4: printf("Wed "); break;
		case 5: printf("Thu "); break;
		case 6: printf("Fri "); break;
		case 7: printf("Sat ");
	}

	switch (mon)
	{
		case  1: printf("Jan "); break;
		case  2: printf("Feb "); break;
		case  3: printf("Mar "); break;
		case  4: printf("Apr "); break;
		case  5: printf("May "); break;
		case  6: printf("Jun "); break;
		case  7: printf("Jul "); break;
		case  8: printf("Aug "); break;
		case  9: printf("Sep "); break;
		case 10: printf("Oct "); break;
		case 11: printf("Nov "); break;
		case 12: printf("Dec ");
	}

	printf("%d ", mday);
	printf("%d:", hour);
	printf(min  < 10 ?  "0%d:" :  "%d:", min);
	printf(sec  < 10 ?  "0%d " :  "%d ", sec);
	printf(year < 10 ? "200%d" : "20%d", year);

	printf("\n");
*/
}

/*----------------------------------------------------------------------------*\
 |				 print_uptime()				      |
\*----------------------------------------------------------------------------*/
void print_uptime(void)
{
	struct tms *buffer = do_malloc(sizeof(struct tms));

	unsigned long uptime_in_ticks = do_times(buffer);
	unsigned long  uptime_in_secs = uptime_in_ticks / HZ;

	unsigned char  days = uptime_in_secs                 / SECS_PER_DAY;
	unsigned char hours = uptime_in_secs % SECS_PER_DAY  / SECS_PER_HOUR;
	unsigned char  mins = uptime_in_secs % SECS_PER_HOUR / SECS_PER_MIN;
	unsigned char  secs = uptime_in_secs % SECS_PER_MIN;

	do_free(buffer);
	printf("up: %d day(s), %d hour(s), %d minute(s), %d second(s)\n",
		days, hours, mins, secs);
}

/*----------------------------------------------------------------------------*\
 |				print_emoticon()			      |
\*----------------------------------------------------------------------------*/
void print_emoticon(unsigned char emoticon)
{
	set_attr(SOLID, IMPORTANT_BG, IMPORTANT_FG);
	switch (emoticon)
	{
		case ERASE: printf("\b\b\b"); break;
		case SMILE: printf(":-)");    break;
		case THINK: printf(":-\\");   break;
		case FROWN: printf(":-(");    break;
	}
	set_attr(SOLID, NORMAL_BG, NORMAL_FG);
}

/*----------------------------------------------------------------------------*\
 |				   print_op()				      |
\*----------------------------------------------------------------------------*/
void print_op(unsigned char op)
{
	switch (op)
	{
		case SYS_SBRK        : printf("sbrk");        break;
		case SYS_FORK        : printf("fork");        break;
		case SYS_EXECVE      : printf("execve");      break;
		case SYS_EXIT        : printf("exit");        break;
		case SYS_GETPGRP     : printf("getpgrp");     break;
		case SYS_GETPID      : printf("getpid");      break;
		case SYS_SETSID      : printf("setsid");      break;
		case SYS_WAIT        : printf("wait");        break;
		case SYS_WAITPID     : printf("waitpid");     break;

		case SYS_ALARM       : printf("alarm");       break;
		case SYS_KILL        : printf("kill");        break;
		case SYS_PAUSE       : printf("pause");       break;
		case SYS_SIGACTION   : printf("sigaction");   break;
		case SYS_SIGPENDING  : printf("sigpending");  break;
		case SYS_SIGPROCMASK : printf("sigprocmask"); break;

		case SYS_ACCESS      : printf("access");      break;
		case SYS_CLOSE       : printf("close");       break;
		case SYS_CREAT       : printf("creat");       break;
		case SYS_DUP         : printf("dup");         break;
		case SYS_DUP2        : printf("dup2");        break;
		case SYS_FCNTL       : printf("fcntl");       break;
		case SYS_FSTAT       : printf("fstat");       break;
		case SYS_IOCTL       : printf("ioctl");       break;
		case SYS_LSEEK       : printf("lseek");       break;
		case SYS_MKNOD       : printf("mknod");       break;
		case SYS_OPEN        : printf("open");        break;
		case SYS_PIPE        : printf("pipe");        break;
		case SYS_READ        : printf("read");        break;
		case SYS_RENAME      : printf("rename");      break;
		case SYS_STAT        : printf("stat");        break;
		case SYS_WRITE       : printf("write");       break;

		case SYS_CHDIR       : printf("chdir");       break;
		case SYS_CHROOT      : printf("chroot");      break;
		case SYS_LINK        : printf("link");        break;
		case SYS_MKDIR       : printf("mkdir");       break;
		case SYS_RMDIR       : printf("rmdir");       break;
		case SYS_SYNC        : printf("sync");        break;
		case SYS_UNLINK      : printf("unlink");      break;

		case SYS_CHMOD       : printf("chmod");       break;
		case SYS_CHOWN       : printf("chown");       break;
		case SYS_GETGID      : printf("getgid");      break;
		case SYS_GETUID      : printf("getuid");      break;
		case SYS_SETGID      : printf("setgid");      break;
		case SYS_SETUID      : printf("setuid");      break;
		case SYS_UMASK       : printf("umask");       break;

		case SYS_TIME        : printf("time");        break;
		case SYS_TIMES       : printf("times");       break;
		case SYS_UTIME       : printf("utime");       break;

		case REGISTER        : printf("register");    break;
		case IRQ             : printf("IRQ");         break;
		case FREQUENCY       : printf("frequency");   break;
		case WATCHDOG        : printf("watchdog");    break;
		case UPTIME          : printf("uptime");      break;
		case SHUTDOWN        : printf("shutdown");    break;
	}
}

/*----------------------------------------------------------------------------*\
 |				  print_pid()				      |
\*----------------------------------------------------------------------------*/
void print_pid(pid_t pid)
{
	switch (pid)
	{
		case     ANYONE : printf("anyone");        break;
		case   HARDWARE : printf("hardware");      break;
		case   IDLE_PID : printf("idle");          break;
		case KERNEL_PID : printf("kernel");        break;
		case    TMR_PID : printf("timer");         break;
		case     FS_PID : printf("file system");   break;
		case    MEM_PID : printf("memory");        break;
		case    FDC_PID : printf("floppy");        break;
		case    TTY_PID : printf("terminal");      break;
		default         : printf("user(%d)", pid); break;
	}
}
