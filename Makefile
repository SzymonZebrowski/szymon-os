# $@ = target file
# $< = first dependency
# $^ = all dependencies

C_SOURCES = $(wildcard kernel/*.c drivers/*.c cpu/*.c libc/*.c)
HEADERS = $(wildcard kernel/*.h drivers/*.h cpu/*.h libc/*.h)
OBJ = ${C_SOURCES:.c=.o cpu/interrupt.o}

CC = ~/opt/cross/bin/i686-elf-gcc
GDB = ~/opt/cross/bin/i686-elf-gdb
LD = ~/opt/cross/bin/i686-elf-ld

CFLAGS = -g


os-image.bin: boot/32bit-main.bin kernel.bin
	cat $^ > $@

kernel.bin: boot/kernel_entry.o ${OBJ}
	${LD} -T link.ld -o $@ -Ttext 0x1000 $^ --oformat binary

kernel.elf: boot/kernel_entry.o ${OBJ}
	${LD} -T link.ld -o $@ -Ttext 0x1000 $^ 

run: os-image.bin
	cp $< os.img
	qemu-system-i386 -fda $< -hda pnd.img -boot d

debug: os-image.bin kernel.elf
	qemu-system-i386 -s -fda os-image.bin -hda pnd.img -boot d -d guest_errors,int &
	gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.elf"

%.o: %.c ${HEADERS}
	${CC} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
	nasm $< -f elf -o $@

%.bin: %.asm
	nasm $< -f bin -o $@

clean:
	rm -rf *.bin *.dis *.o os-image.bin *.elf os.img
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o cpu/*.o libc/*.o