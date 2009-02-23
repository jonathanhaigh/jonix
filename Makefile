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
	lib/util/string.c 	\
	lib/oarray.c		\
	lib/mem.c			\
	lib/heap.c			\
	lib/io.c 			\
	lib/scrn.c   		\
	lib/gdt.c 			\
	lib/idt.c 			\
	lib/isr.c 			\
	lib/irq.c 			\
	lib/time.c			\
	lib/list.c			\
	lib/pci.c			\
	lib/ata.c			\
	lib/kb.c 			\
	lib/tid.c			\
	lib/sem.c			\
	lib/thr.c			\
	lib/sched.c			\
	main.c				\

ASM_FILES=boot.asm

OBJ_FILES=$(ASM_FILES:.asm=.o) $(C_FILES:.c=.o)

CC=gcc

INCLUDE=					\
	-Ilib 					\
	-Ilib/c 				\
	-Ilib/util

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

kernel.elf: $(OBJ_FILES) linker.ld
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
lib/c/printf.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stddef.h lib/scrn.h
lib/c/printf.o: lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/util/string.o: lib/util/util.h lib/c/stddef.h lib/c/stdint.h
lib/oarray.o: lib/oarray.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/mem.o: lib/mem.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/oarray.h
lib/mem.o: lib/c/string.h lib/scrn.h
lib/heap.o: lib/mem.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/oarray.h
lib/heap.o: lib/c/string.h lib/scrn.h
lib/io.o: lib/io.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/scrn.o: lib/scrn.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/scrn.o: lib/c/string.h lib/util/util.h lib/io.h
lib/gdt.o: lib/gdt.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/gdt.o: lib/c/string.h lib/mem.h lib/oarray.h
lib/idt.o: lib/idt.h lib/system.h lib/c/stddef.h lib/c/stdint.h
lib/idt.o: lib/c/string.h lib/system_boot.h
lib/isr.o: lib/isr.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/idt.h
lib/isr.o: lib/scrn.h
lib/irq.o: lib/irq.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/isr.h
lib/irq.o: lib/io.h lib/idt.h
lib/time.o: lib/time.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/isr.h
lib/time.o: lib/sched.h lib/io.h lib/irq.h
lib/list.o: lib/list.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/mem.h
lib/list.o: lib/oarray.h
lib/pci.o: lib/pci.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/list.h
lib/pci.o: lib/scrn.h lib/mem.h lib/oarray.h lib/util/util.h lib/io.h
lib/ata.o: lib/ata.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/sem.h
lib/ata.o: lib/list.h lib/isr.h lib/io.h lib/pci.h lib/irq.h lib/time.h
lib/ata.o: lib/c/string.h lib/scrn.h lib/mem.h lib/oarray.h lib/util/util.h
lib/kb.o: lib/kb.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/scrn.h
lib/kb.o: lib/irq.h lib/isr.h lib/io.h lib/c/string.h
lib/tid.o: lib/tid.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/sem.h
lib/tid.o: lib/list.h lib/thr.h lib/mem.h lib/oarray.h
lib/sem.o: lib/sem.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/list.h
lib/sem.o: lib/thr.h
lib/thr.o: lib/thr.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/list.h
lib/thr.o: lib/tid.h lib/mem.h lib/oarray.h lib/c/string.h lib/scrn.h
lib/sched.o: lib/thr.h lib/system.h lib/c/stddef.h lib/c/stdint.h lib/list.h
lib/sched.o: lib/sched.h lib/isr.h
main.o: lib/c/stdio.h lib/c/stdarg.h lib/c/stddef.h lib/system.h
main.o: lib/c/stddef.h lib/c/stdint.h lib/system_boot.h lib/ata.h lib/sem.h
main.o: lib/list.h lib/isr.h lib/pci.h lib/util/util.h lib/scrn.h lib/mem.h
main.o: lib/oarray.h lib/gdt.h lib/irq.h lib/idt.h lib/kb.h lib/time.h
main.o: lib/debug.h
