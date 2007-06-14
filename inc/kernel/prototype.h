/*----------------------------------------------------------------------------*\
 |	prototype.h							      |
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



#ifndef _KERNEL_PROTOTYPE_H
#define _KERNEL_PROTOTYPE_H



/*----------------------------------------------------------------------------*\
 |		 Available Within the Kernel and System Tasks:		      |
\*----------------------------------------------------------------------------*/

/* assembly.S */
unsigned char in_byte(unsigned short port);
void out_byte(unsigned char value, unsigned short port);
void io_delay(unsigned char num_delays);

/* keyboard.c */
void kbd_leds(bool caps, bool num, bool scroll);
void kbd_init(void);

/* malloc.c */
void *kmalloc(size_t size);
void *do_malloc(size_t size);
void kfree(void *ptr);
void do_free(void *ptr);

/* message.c */
msg_t *msg_alloc(pid_t to, unsigned char op);
void msg_free(msg_t *msg);
msg_t *msg_check(mid_t mid);
msg_t *msg_receive(mid_t mid);
void msg_send(msg_t *msg);
msg_t *msg_send_receive(msg_t *msg);
void msg_reply(msg_t *msg);

/* panic.c */
void panic(char *func, char *msg);

/* process.c */
id_t proc_info(pid_t pid, unsigned char request);
void proc_nap(void);

/* syscall.c */
pid_t do_fork(void);
int do_execve(const char *path, char *const argv[], char *const envp[]);
void do_exit(int status);
gid_t do_getegid(void);
uid_t do_geteuid(void);
gid_t do_getgid(void);
pid_t do_getpgrp(void);
pid_t do_getpid(void);
pid_t do_getppid(void);
uid_t do_getuid(void);
pid_t do_setpgrp(void);
time_t do_time(time_t *tloc);



/*----------------------------------------------------------------------------*\
 |		       Available Within Only the Kernel:		      |
\*----------------------------------------------------------------------------*/

#ifdef BRAINIX

/* assembly.S */
void load_gdtr(void);
void load_idtr(void);
void disable_intrs(void);
void enable_intrs(void);
unsigned long save_dir(void);
void load_dir(unsigned long pg_dir);
void invalidate_tlbs(void);
void disable_paging(void);
void enable_paging(void);
unsigned long get_fault_addr(void);
void halt(void);

/* interrupt.c */
void intr_set(unsigned char vector, unsigned long addr, unsigned char dpl);
void intr_init(void);
void intr_lock(void);
void intr_unlock(void);

/* irq.c */
void irq_disable(unsigned char num);
void irq_enable(unsigned char num);
void irq_init(void);
void irq_eoi(unsigned char num);
void irq_register(msg_t *msg);

/* keyboard.c */
void kbd_reboot(void);

/* malloc.c */
bool heap_pg_fault(bool kernel, unsigned long lin);
bool heap_init(unsigned long target_pg_dir, bool kernel);
void *do_sbrk(ptrdiff_t increment);

/* message.c */
void msg_init(void);
void msg_empty(pid_t pid);

/* output.c */
void print_version(void);
void print_time(void);
void print_uptime(void);
void print_emoticon(unsigned char emoticon);
void print_op(unsigned char op);
void print_pid(pid_t pid);

/* paging.c */
void paging_init(unsigned long mem_upper);
unsigned long pg_space_create(void);
void pg_space_destroy(unsigned long pg_dir);
unsigned long pg_addr(bool kernel, bool heap, bool bottom);
unsigned long pg_next_addr(unsigned long pg_dir, bool kernel, bool heap);
unsigned long pg_alloc(unsigned long pg_dir, bool kernel, bool heap);
void pg_free(unsigned long pg_dir, unsigned long lin);
void pg_fork(unsigned long pg_dir_1, unsigned long pg_dir_2);
void pg_exec(unsigned long pg_dir_1, unsigned long pg_dir_2);

/* process.c */
void proc_init(void);
pid_t proc_create(unsigned long entry, bool kernel);
void proc_destroy(pid_t pid);
void proc_sleep(pid_t pid);
void proc_wakeup(pid_t pid);
void proc_sched(void);
void proc_fork(msg_t *msg);
void proc_exec(msg_t *msg);
void proc_exit(msg_t *msg);
void proc_time(bool kernel);
void proc_update(void);
int proc_setpgrp(void);
clock_t do_times(struct tms *buffer);

/* segment.c */
void seg_init(void);

/* signal.c */
void sig_init(pid_t pid);
void sig_deliver(void);
void sig_send(pid_t pid, int sig);
int do_kill(pid_t pid, int sig);
int do_pause(void);
int do_sigaction(int sig, const struct sigaction *act, struct sigaction *oact);
int do_sigpending(sigset_t *set);
int do_sigprocmask(int how, const sigset_t *set, sigset_t *oset);

/* stack.c */
bool stack_pg_fault(bool kernel, unsigned long fault_addr);
bool stack_init(unsigned long pg_dir, bool kernel);

/* timer.c */
void timer_main(void);

/* wrapper.S */
void default_wrapper(void);
void wrapper__0(void);
void wrapper__1(void);
void wrapper__2(void);
void wrapper__3(void);
void wrapper__4(void);
void wrapper__5(void);
void wrapper__6(void);
void wrapper__7(void);
void wrapper__8(void);
void wrapper__9(void);
void wrapper_10(void);
void wrapper_11(void);
void wrapper_12(void);
void wrapper_13(void);
void wrapper_14(void);
void wrapper_16(void);
void wrapper_17(void);
void wrapper_18(void);
void wrapper_19(void);
void timer_wrapper(void);
void kbd_wrapper(void);
void wrapper_32(void);
void wrapper_33(void);
void wrapper_34(void);
void wrapper_35(void);
void wrapper_36(void);
void wrapper_37(void);
void wrapper_38(void);
void wrapper_39(void);
void wrapper_40(void);
void wrapper_41(void);
void wrapper_42(void);
void wrapper_43(void);
void wrapper_44(void);
void wrapper_45(void);
void wrapper_46(void);
void wrapper_47(void);
void sys_wrapper(void);

#endif



#endif
