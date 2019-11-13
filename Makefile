# $@ = target file
# $< = first dependency
# $^ = all dependencies

all: run

FOLDER = ~/opt/cross/bin

kernel/kernel.bin: boot/kernel_entry.o kernel/kernel.o
	$(FOLDER)/i686-elf-ld -o $@ -Ttext 0x1000 $^ --oformat binary

boot/kernel_entry.o: boot/kernel_entry.asm
	nasm $< -f elf -o $@

kernel/kernel.o: kernel/kernel.c
	$(FOLDER)/i686-elf-gcc -ffreestanding -c $< -o $@

kernel.dis: kernel/kernel.bin
	ndisasm -b 32 $< > $@

boot/bootsect.bin: boot/32bit-main.asm
	nasm -f bin $< -o $@

os-image.bin: boot/bootsect.bin kernel/kernel.bin
	cat $^ > $@

run: os-image.bin
	qemu-system-i386 -fda $<

clean:
	rm {kernel,boot}/{*.bin,*.o,*.dis}