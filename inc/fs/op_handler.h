#include <fs/fs.h>
#include <kernel/kernel.h>
#include <stdbool.h>

#ifndef OP_HANDLER_H
#define OP_HANDLER_H

void fs_op(msg_t *msg);
void fs__execve(msg_t *msg);
void fs__exit(msg_t *msg);
void fs__fork(msg_t *msg);
void fs__access(msg_t *msg);
void fs__close(msg_t *msg);
void fs__dup(msg_t *msg);
void fs__dup2(msg_t *msg);
void fs__FCNTL(msg_t *msg);
void lazy_fstat(msg_t *msg);
void  fs__IOCTL(msg_t *msg);
void  fs__LSEEK(msg_t *msg);
void  fs__OPEN(msg_t *msg);
void  fs__READ(msg_t *msg);
void  fs__STAT(msg_t *msg);
void  fs__WRITE(msg_t *msg);
void  fs__CHDIR(msg_t *msg);
void  fs__CHROOT(msg_t *msg);
void  fs__LINK(msg_t *msg);
void  fs__SYNC(msg_t *msg);
void  fs__UNLINK(msg_t *msg);
void  fs__CHMOD(msg_t *msg);
void  fs__CHOWN(msg_t *msg);
void  fs__UMASK(msg_t *msg);
void  fs__UTIME(msg_t *msg);
void  fs__REGISTER(msg_t *msg);

#endif
