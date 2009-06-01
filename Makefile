# ---------------------------------------------------
# Jonix
#
# main.c
#
# Author(s): Jonathan Haigh
# Last edited by: Jonathan Haigh
# --------------------------------------------------*/

C_FILES=				\
	lib/c/string.c 		\
	lib/c/printf.c		\
	lib/util/oarray.c	\
	lib/mem.c			\
	lib/heap.c			\
	lib/io.c 			\
	lib/devs/scrn.c   	\
	lib/gdt.c 			\
	lib/idt.c 			\
	lib/isr.c 			\
	lib/irq.c 			\
	lib/time.c			\
	lib/util/list.c		\
	lib/devs/pci.c		\
	lib/devs/ata.c		\
    lib/devs/ptbl.c     \
	lib/devs/kb.c 		\
    lib/devs/ramdisk.c  \
	lib/sem.c			\
	lib/thr.c			\
	lib/sched.c			\
    lib/syscall.c       \
    lib/user/unistd.c   \
	main.c				\

ASM_FILES=boot.asm

OBJ_FILES=$(ASM_FILES:.asm=.o) $(C_FILES:.c=.o)

CC=gcc

INCLUDE=					\
	-Ilib 					\

CFLAGS=						\
	-fno-stack-protector 	\
	-fno-hosted				\
	-nostdlib 				\
	-nostartfiles 			\
	-nodefaultlibs 			\
	-Wall 					\
	-Werror 				\
	-m32	 				\
	$(INCLUDE)

kernel.elf: depend $(OBJ_FILES) linker.ld
	ld -melf_i386 -T linker.ld $(OBJ_FILES) -o kernel.elf

ldv: $(OBJ_FILES) linker.ld
	ld -melf_i386 -M -T linker.ld $(OBJ_FILES) -o kernel.elf

boot.o: boot.asm
	nasm -f elf -o boot.o boot.asm

clean:
	-rm $(OBJ_FILES) kernel.elf

install: kernel.elf
	cp kernel.elf /boot/jonix.elf
	mount -o loop /home/jonathan/.bochs/floppy.img /mnt/tmp
	cp kernel.elf /mnt/tmp/jonix.elf
	umount /mnt/tmp

test:
	qbg bochs

depend:
	makedepend $(C_FILES) $(INCLUDE)
# DO NOT DELETE

lib/c/string.o: lib/c/string.h lib/c/stddef.h lib/c/stdint.h
lib/c/printf.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stddef.h lib/devs/scrn.h
lib/c/printf.o: lib/system.h lib/c/stdint.h lib/c/stdbool.h lib/c/string.h
lib/c/printf.o: lib/user/sys/types.h lib/debug.h
lib/util/oarray.o: lib/util/oarray.h lib/system.h lib/c/stddef.h
lib/util/oarray.o: lib/c/stdint.h lib/c/stdio.h lib/c/stdarg.h
lib/util/oarray.o: lib/c/stdbool.h lib/c/string.h lib/user/sys/types.h
lib/util/oarray.o: lib/debug.h lib/util/sort.h
lib/mem.o: lib/mem.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/c/stdio.h
lib/mem.o: lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h lib/user/sys/types.h
lib/mem.o: lib/debug.h lib/util/oarray.h lib/util/sort.h lib/devs/scrn.h
lib/heap.o: lib/mem.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/heap.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h
lib/heap.o: lib/user/sys/types.h lib/debug.h lib/util/oarray.h
lib/heap.o: lib/util/sort.h lib/devs/scrn.h
lib/io.o: lib/io.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/c/stdio.h
lib/io.o: lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h lib/user/sys/types.h
lib/io.o: lib/debug.h
lib/devs/scrn.o: lib/devs/scrn.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/devs/scrn.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h
lib/devs/scrn.o: lib/user/sys/types.h lib/debug.h lib/io.h
lib/gdt.o: lib/gdt.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/c/stdio.h
lib/gdt.o: lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h lib/user/sys/types.h
lib/gdt.o: lib/debug.h lib/mem.h lib/util/oarray.h lib/util/sort.h lib/thr.h
lib/gdt.o: lib/util/list.h lib/isr.h
lib/idt.o: lib/idt.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/c/stdio.h
lib/idt.o: lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h lib/user/sys/types.h
lib/idt.o: lib/debug.h lib/system_boot.h
lib/isr.o: lib/isr.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/c/stdio.h
lib/isr.o: lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h lib/user/sys/types.h
lib/isr.o: lib/debug.h lib/idt.h lib/devs/scrn.h
lib/irq.o: lib/irq.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/c/stdio.h
lib/irq.o: lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h lib/user/sys/types.h
lib/irq.o: lib/debug.h lib/isr.h lib/io.h lib/idt.h
lib/time.o: lib/time.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/time.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h
lib/time.o: lib/user/sys/types.h lib/debug.h lib/isr.h lib/thr.h
lib/time.o: lib/util/list.h lib/util/sort.h lib/mem.h lib/util/oarray.h
lib/time.o: lib/sched.h lib/io.h lib/irq.h
lib/util/list.o: lib/util/list.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/util/list.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h
lib/util/list.o: lib/user/sys/types.h lib/debug.h lib/util/sort.h lib/mem.h
lib/util/list.o: lib/util/oarray.h
lib/devs/pci.o: lib/devs/pci.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/devs/pci.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h
lib/devs/pci.o: lib/user/sys/types.h lib/debug.h lib/util/list.h
lib/devs/pci.o: lib/util/sort.h lib/devs/scrn.h lib/mem.h lib/util/oarray.h
lib/devs/pci.o: lib/io.h lib/devs/pci_dev_info.h
lib/devs/ata.o: lib/devs/ata.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/devs/ata.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h
lib/devs/ata.o: lib/user/sys/types.h lib/debug.h lib/sem.h lib/util/list.h
lib/devs/ata.o: lib/util/sort.h lib/isr.h lib/io.h lib/devs/pci.h lib/irq.h
lib/devs/ata.o: lib/time.h lib/thr.h lib/mem.h lib/util/oarray.h
lib/devs/ata.o: lib/devs/ptbl.h lib/filesystems/vfs.h lib/devs/hal.h
lib/devs/ata.o: lib/util/id.h
lib/devs/ptbl.o: lib/system.h lib/c/stddef.h lib/c/stdint.h lib/c/stdio.h
lib/devs/ptbl.o: lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h
lib/devs/ptbl.o: lib/user/sys/types.h lib/debug.h lib/devs/ptbl.h
lib/devs/ptbl.o: lib/util/list.h lib/util/sort.h lib/filesystems/vfs.h
lib/devs/ptbl.o: lib/devs/hal.h lib/util/id.h lib/mem.h lib/util/oarray.h
lib/devs/kb.o: lib/devs/kb.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/devs/kb.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h
lib/devs/kb.o: lib/user/sys/types.h lib/debug.h lib/devs/scrn.h lib/irq.h
lib/devs/kb.o: lib/isr.h lib/io.h
lib/devs/ramdisk.o: lib/devs/ramdisk.h lib/system.h lib/c/stddef.h
lib/devs/ramdisk.o: lib/c/stdint.h lib/c/stdio.h lib/c/stdarg.h
lib/devs/ramdisk.o: lib/c/stdbool.h lib/c/string.h lib/user/sys/types.h
lib/devs/ramdisk.o: lib/debug.h lib/devs/hal.h lib/util/id.h lib/mem.h
lib/devs/ramdisk.o: lib/util/oarray.h lib/util/sort.h
lib/sem.o: lib/sem.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/c/stdio.h
lib/sem.o: lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h lib/user/sys/types.h
lib/sem.o: lib/debug.h lib/util/list.h lib/util/sort.h lib/thr.h lib/mem.h
lib/sem.o: lib/util/oarray.h lib/isr.h lib/sched.h
lib/thr.o: lib/thr.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/c/stdio.h
lib/thr.o: lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h lib/user/sys/types.h
lib/thr.o: lib/debug.h lib/util/list.h lib/util/sort.h lib/mem.h
lib/thr.o: lib/util/oarray.h lib/isr.h lib/devs/scrn.h lib/gdt.h lib/time.h
lib/thr.o: lib/user/unistd.h
lib/sched.o: lib/thr.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/sched.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h
lib/sched.o: lib/user/sys/types.h lib/debug.h lib/util/list.h lib/util/sort.h
lib/sched.o: lib/mem.h lib/util/oarray.h lib/isr.h lib/sched.h lib/irq.h
lib/sched.o: lib/gdt.h
lib/syscall.o: lib/syscall.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/syscall.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stdbool.h lib/c/string.h
lib/syscall.o: lib/user/sys/types.h lib/debug.h lib/isr.h lib/time.h
lib/syscall.o: lib/thr.h lib/util/list.h lib/util/sort.h lib/mem.h
lib/syscall.o: lib/util/oarray.h lib/idt.h
lib/user/unistd.o: lib/user/unistd.h lib/c/stdint.h
main.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stddef.h lib/system.h
main.o: lib/c/stdint.h lib/c/stdbool.h lib/c/string.h lib/user/sys/types.h
main.o: lib/debug.h lib/system_boot.h lib/devs/ata.h lib/sem.h
main.o: lib/util/list.h lib/util/sort.h lib/isr.h lib/devs/pci.h
main.o: lib/devs/scrn.h lib/mem.h lib/util/oarray.h lib/gdt.h lib/irq.h
main.o: lib/idt.h lib/devs/kb.h lib/time.h lib/thr.h lib/syscall.h
main.o: lib/c/fcntl.h lib/filesystems/vfs.h lib/devs/hal.h lib/util/id.h
