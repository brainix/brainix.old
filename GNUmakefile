#==============================================================================#
#	GNUmakefile							       #
#									       #
#	Copyright © 2002-2007, Team Brainix, original authors.		       #
#		All rights reserved.					       #
#==============================================================================#

FDDEV = /dev/fd0
FDMNT = /mnt/floppy
IMG   = /home/rajiv/tmp/emulator/bootdisk.img

CC      = gcc
CFLAGS  = -ffreestanding -fno-stack-protector -O0 -Iinc -Iinc/lib -march=i386
LDFLAGS = -nostdlib -Wl,-Ttext -Wl,100000

KERNEL = bin/kernel/boot.o	\
	bin/kernel/assembly.o	\
	bin/kernel/clock.o	\
	bin/kernel/domino.o	\
	bin/kernel/exception.o	\
	bin/kernel/interrupt.o	\
	bin/kernel/irq.o	\
	bin/kernel/keyboard.o	\
	bin/kernel/main.o	\
	bin/kernel/malloc.o	\
	bin/kernel/message.o	\
	bin/kernel/output.o	\
	bin/kernel/paging.o	\
	bin/kernel/panic.o	\
	bin/kernel/process.o	\
	bin/kernel/segment.o	\
	bin/kernel/signal.o	\
	bin/kernel/stack.o	\
	bin/kernel/syscall.o	\
	bin/kernel/task.o	\
	bin/kernel/timer.o	\
	bin/kernel/wrapper.o

FS = bin/fs/block.o		\
	bin/fs/device.o		\
	bin/fs/dir.o		\
	bin/fs/elf.o		\
	bin/fs/fildes.o		\
	bin/fs/group.o		\
	bin/fs/inode.o		\
	bin/fs/link.o		\
	bin/fs/main.o		\
	bin/fs/misc.o		\
	bin/fs/mount.o		\
	bin/fs/open.o		\
	bin/fs/path.o		\
	bin/fs/perm.o		\
	bin/fs/proc.o		\
	bin/fs/read.o		\
	bin/fs/search.o		\
	bin/fs/stat.o		\
	bin/fs/super.o		\
	bin/fs/time.o

DRIVER = bin/driver/dma.o	\
	bin/driver/driver.o	\
	bin/driver/floppy.o	\
	bin/driver/keyboard.o	\
	bin/driver/memory.o	\
	bin/driver/network.o	\
	bin/driver/speaker.o	\
	bin/driver/terminal.o	\
	bin/driver/video.o

LIB = bin/lib/signal.o		\
	bin/lib/string.o	\
	bin/lib/sys/stat.o	\
	bin/lib/sys/times.o	\
	bin/lib/syscall.o	\
	bin/lib/time.o		\
	bin/lib/unistd.o	\
	bin/lib/utime.o

all : bin/Brainix

clean :
	rm -f $(KERNEL) $(FS) $(DRIVER) $(LIB) bin/Brainix

bin/%.o : src/%.S
	$(CC) -c -o $@ $< $(CFLAGS)

bin/%.o : src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

bin/Brainix : $(KERNEL) $(FS) $(DRIVER) $(LIB)
	$(CC) -o $@ $(LDFLAGS) $(KERNEL) $(FS) $(DRIVER) $(LIB)

floppy :
	dd if=Bootdisk.img of=$(FDDEV)
	mount $(FDDEV) $(FDMNT)
	cp bin/Brainix $(FDMNT)
	gzip $(FDMNT)/Brainix
	umount $(FDDEV)

Bochs :
	mount -o loop $(IMG) $(FDMNT)
	cp bin/Brainix $(FDMNT)
	gzip -f $(FDMNT)/Brainix
	umount $(FDMNT)
