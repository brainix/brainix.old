/*----------------------------------------------------------------------------*\
 |	main.c								      |
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

#include <fs/fs.h>

void leaky_free(char ***p);

/*----------------------------------------------------------------------------*\
 |				   fs_main()				      |
\*----------------------------------------------------------------------------*/
void fs_main(void)
{
	/* Initialize the file system. */
	block_init(); /* Initialize the block cache.                      */
	inode_init(); /* Initialize the inode table.                      */
	super_init(); /* Initialize the superblock table.                 */
	dev_init();   /* Initialize the device driver PID table.          */
	descr_init(); /* Init the file ptr and proc-specific info tables. */

	/* Wait for a message. */
	while ((msg = msg_receive(ANYONE))->op != SHUTDOWN)
	{
		super_user = proc_info(msg->from, EUID) == 0;
		err_code = 0;

		/* Do the requested work. */
		switch (msg->op)
		{
			case SYS_EXECVE:
				msg->args.execve.ret_val =
					do_fs_execve(msg->args.execve.path,
						     msg->args.execve.argv,
						     msg->args.execve.envp);
				break;
			case SYS_EXIT:
					do_fs_exit(msg->args.exit.status);
				break;
			case SYS_FORK:
					do_fs_fork(msg->from,
						   msg->args.fork.ret_val);
				break;
			case SYS_ACCESS:
				msg->args.access.ret_val =
					do_fs_access(msg->args.access.path,
						     msg->args.access.amode);
				break;
			case SYS_CLOSE:
				msg->args.close.ret_val =
					do_fs_close(msg->args.close.fildes);
				break;
			case SYS_DUP:
				msg->args.dup.ret_val =
					do_fs_dup(msg->args.dup.fildes);
				break;
			case SYS_DUP2:
				msg->args.dup2.ret_val =
					do_fs_dup2(msg->args.dup2.fildes,
						   msg->args.dup2.fildes2);
				break;
			case SYS_FCNTL:
				msg->args.fcntl.ret_val =
					do_fs_fcntl(msg->args.fcntl.fildes,
						    msg->args.fcntl.cmd,
						    msg->args.fcntl.arg);
				break;
			case SYS_FSTAT:
				msg->args.fstat.ret_val =
					do_fs_fstat(msg->args.fstat.fildes,
						    msg->args.fstat.buf);
				break;
			case SYS_IOCTL:
				msg->args.ioctl.ret_val =
					do_fs_ioctl(msg->args.ioctl.fildes,
						    msg->args.ioctl.request,
						    msg->args.ioctl.arg);
				break;
			case SYS_LSEEK:
				msg->args.lseek.ret_val =
					do_fs_lseek(msg->args.lseek.fildes,
						    msg->args.lseek.offset,
						    msg->args.lseek.whence);
				break;
			case SYS_OPEN:
				msg->args.open.ret_val =
					do_fs_open(msg->args.open.path,
						   msg->args.open.oflag,
						   msg->args.open.mode);
				break;
			case SYS_READ:
				msg->args.read.ret_val =
					do_fs_read(msg->args.read.fildes,
						   msg->args.read.buf,
						   msg->args.read.nbyte);
				break;
			case SYS_STAT:
				msg->args.stat.ret_val =
					do_fs_stat(msg->args.stat.path,
						   msg->args.stat.buf);
				break;
			case SYS_WRITE:
				msg->args.write.ret_val =
					do_fs_write(msg->args.write.fildes,
						    msg->args.write.buf,
						    msg->args.write.nbyte);
				break;
			case SYS_CHDIR:
				msg->args.chdir.ret_val =
					do_fs_chdir(msg->args.chdir.path);
				break;
			case SYS_CHROOT:
				msg->args.chroot.ret_val =
					do_fs_chroot(msg->args.chroot.path);
				break;
			case SYS_LINK:
				msg->args.link.ret_val =
					do_fs_link(msg->args.link.path1,
						   msg->args.link.path2);
				break;
			case SYS_SYNC:
					do_fs_sync();
				break;
			case SYS_UNLINK:
				msg->args.unlink.ret_val =
					do_fs_unlink(msg->args.unlink.path);
				break;
			case SYS_CHMOD:
				msg->args.chmod.ret_val =
					do_fs_chmod(msg->args.chmod.path,
						    msg->args.chmod.mode);
				break;
			case SYS_CHOWN:
				msg->args.chown.ret_val =
					do_fs_chown(msg->args.chown.path,
						    msg->args.chown.owner,
						    msg->args.chown.group);
				break;
			case SYS_UMASK:
				msg->args.umask.ret_val =
					do_fs_umask(msg->args.umask.cmask);
				break;
			case SYS_UTIME:
				msg->args.utime.ret_val =
					do_fs_utime(msg->args.utime.path,
						    msg->args.utime.times);
				break;
			case REGISTER:
					fs_register(msg->args.brnx_reg.block,
						    msg->args.brnx_reg.maj,
						    msg->from);
				break;
			default:
				panic("fs_main", "unexpected message");
				break;
		}

		/* Send a reply (if necessary). */
		switch (msg->op)
		{
			case SYS_EXECVE:
				if (msg->args.execve.ret_val == 0)
				{
					kfree(msg->args.execve.path);
					leaky_free(&msg->args.execve.argv);
					leaky_free(&msg->args.execve.envp);
					msg_free(msg);
				}
				else
					msg_reply(msg);
				break;
			case SYS_EXIT:
			case SYS_FORK:
			case SYS_SYNC:
			case REGISTER:
				msg_free(msg);
				break;
			default:
				msg_reply(msg);
		}
	}

	/* Deinitialize the file system. */
	umount_root();
	dev_deinit();
	msg_reply(msg);
	while (true)
		msg_free(msg_receive(ANYONE));
}
