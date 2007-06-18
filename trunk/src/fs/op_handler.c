#include <fs/fs.h>
#include <fs/op_handler.h>

void fs_op(msg_t *msg)
{
	switch (msg->op)
	{
		case SYS_EXECVE:
			fs__execve(msg);
			break;
		case SYS_EXIT:
			fs__exit(msg);
			break;
		case SYS_FORK:
			fs__fork(msg);
			break;
		case SYS_ACCESS:
			fs__access(msg);
			break;
		case SYS_CLOSE:
			fs__close(msg);
			break;
		case SYS_DUP:
			fs__dup(msg);
			break;
		case SYS_DUP2:
			fs__dup2(msg);
			break;
		case SYS_FCNTL:
			fs__FCNTL(msg);
			break;
		case SYS_FSTAT:
			lazy_fstat(msg);
			break;
		case SYS_IOCTL:
			fs__IOCTL(msg);
			break;
		case SYS_LSEEK:
			fs__LSEEK(msg);
			break;
		case SYS_OPEN:
			fs__OPEN(msg);
			break;
		case SYS_READ:
			fs__READ(msg);
			break;
		case SYS_STAT:
			fs__STAT(msg);
			break;
		case SYS_WRITE:
			fs__WRITE(msg);
			break;
		case SYS_CHDIR:
			fs__CHDIR(msg);
			break;
		case SYS_CHROOT:
			fs__CHROOT(msg);
			break;
		case SYS_LINK:
			fs__LINK(msg);
			break;
		case SYS_SYNC:
			fs__SYNC(msg);
			break;
		case SYS_UNLINK:
			fs__UNLINK(msg);	
			break;
		case SYS_CHMOD:
			fs__CHMOD(msg);
			break;
		case SYS_CHOWN:
			fs__CHOWN(msg);
			break;
		case SYS_UMASK:
			fs__UMASK(msg);
			break;
		case SYS_UTIME:
			fs__UTIME(msg);
			break;
		case REGISTER:
			fs__REGISTER(msg);
			break;
		default:
			panic("fs_main", "unexpected message");
			break;
	}
	do_exit(0);
}
void fs__execve(msg_t *msg)
{
	msg->args.execve.ret_val =
		do_fs_execve(msg->args.execve.path,
			     msg->args.execve.argv,
			     msg->args.execve.envp);
	if (msg->args.execve.ret_val == 0)
	{
		kfree(msg->args.execve.path);
		leaky_free(&msg->args.execve.argv);
		leaky_free(&msg->args.execve.envp);
		msg_free(msg);
	}
	else
		msg_reply(msg);
}
void fs__exit(msg_t *msg)
{
	do_fs_exit(msg->args.exit.status);

}
void fs__fork(msg_t *msg)
{
	do_fs_fork(msg->from, msg->args.fork.ret_val);
}

void fs__access(msg_t *msg)
{
		msg->args.access.ret_val =
				do_fs_access(msg->args.access.path,
					     msg->args.access.amode);
			msg_reply(msg);
}
void fs__close(msg_t *msg) 
{
	msg->args.close.ret_val = do_fs_close(msg->args.close.fildes);
	msg_reply(msg);
}
void fs__dup(msg_t *msg)
{
	msg->args.dup.ret_val = do_fs_dup(msg->args.dup.fildes);
	msg_reply(msg);
}
void fs__dup2(msg_t *msg)
{
	msg->args.dup2.ret_val =
		do_fs_dup2(msg->args.dup2.fildes,
			   msg->args.dup2.fildes2);

	msg_reply(msg);
}
void fs__FCNTL(msg_t *msg)
{
	msg->args.fcntl.ret_val =
		do_fs_fcntl(msg->args.fcntl.fildes,
			    msg->args.fcntl.cmd,
			    msg->args.fcntl.arg);
	msg_reply(msg);
}			
void lazy_fstat(msg_t *msg)
{
	msg->args.fstat.ret_val =
		do_fs_fstat(msg->args.fstat.fildes,
			    msg->args.fstat.buf);
	msg_reply(msg);
}
void  fs__IOCTL(msg_t *msg)
{
	msg->args.ioctl.ret_val =
		do_fs_ioctl(msg->args.ioctl.fildes,
			    msg->args.ioctl.request,
			    msg->args.ioctl.arg);
	msg_reply(msg);
}
void  fs__LSEEK(msg_t *msg)
{
	msg->args.lseek.ret_val =
		do_fs_lseek(msg->args.lseek.fildes,
			    msg->args.lseek.offset,
			    msg->args.lseek.whence);
	msg_reply(msg);
}
void  fs__OPEN(msg_t *msg)
{
	msg->args.open.ret_val =
		do_fs_open(msg->args.open.path,
			   msg->args.open.oflag,
			   msg->args.open.mode);
	msg_reply(msg);
}
void  fs__READ(msg_t *msg)
{
	msg->args.read.ret_val =
		do_fs_read(msg->args.read.fildes,
			   msg->args.read.buf,
			   msg->args.read.nbyte);
	msg_reply(msg);
}
void  fs__STAT(msg_t *msg)
{
	msg->args.stat.ret_val =
		do_fs_stat(msg->args.stat.path,
			   msg->args.stat.buf);
	msg_reply(msg);
}
void  fs__WRITE(msg_t *msg)
{
	msg->args.write.ret_val =
		do_fs_write(msg->args.write.fildes,
			    msg->args.write.buf,
			    msg->args.write.nbyte);
	msg_reply(msg);
}
void  fs__CHDIR(msg_t *msg)
{
	msg->args.chdir.ret_val =
		do_fs_chdir(msg->args.chdir.path);
	msg_reply(msg);
}
void  fs__CHROOT(msg_t *msg)
{
	msg->args.chroot.ret_val =
		do_fs_chroot(msg->args.chroot.path);
	msg_reply(msg);
}
void  fs__LINK(msg_t *msg)
{
	msg->args.link.ret_val =
		do_fs_link(msg->args.link.path1,
			   msg->args.link.path2);
	msg_reply(msg);
}
void  fs__SYNC(msg_t *msg)
{
	do_fs_sync();
	msg_reply(msg);
}
void  fs__UNLINK(msg_t *msg)
{
	msg->args.unlink.ret_val =
		do_fs_unlink(msg->args.unlink.path);
	msg_reply(msg);
}
void  fs__CHMOD(msg_t *msg)
{
	msg->args.chmod.ret_val =
		do_fs_chmod(msg->args.chmod.path,
			    msg->args.chmod.mode);
	msg_reply(msg);
}
void  fs__CHOWN(msg_t *msg)
{
	msg->args.chown.ret_val =
		do_fs_chown(msg->args.chown.path,
			    msg->args.chown.owner,
			    msg->args.chown.group);
	msg_reply(msg);
}
void  fs__UMASK(msg_t *msg)
{
	msg->args.umask.ret_val =
		do_fs_umask(msg->args.umask.cmask);
	msg_reply(msg);
}
void  fs__UTIME(msg_t *msg)
{
	msg->args.utime.ret_val =
		do_fs_utime(msg->args.utime.path,
			    msg->args.utime.times);
	msg_reply(msg);
}
void  fs__REGISTER(msg_t *msg)
{
		fs_register(msg->args.brnx_reg.block,
			    msg->args.brnx_reg.maj,
			    msg->from);
	msg_free(msg);
}
