[org 0x7c00]    ;bootloader offset
KERNEL_OFFSET equ 0x1000                 ; offset of kernel, set during linking
    mov [BOOT_DRIVE], dl                 ; during boot, BIOS sets boot drive in dl
    mov bp, 0x9000  ;set the stack
    mov sp, bp

    mov bx, MSG_16B_RM
    call print
    call print_nl

    call load_kernel
    call switch_to_pm

%include "boot/bootsector-print-hex.asm"
%include "boot/bootsector-print.asm"
%include "boot/bootsector-disk-load.asm"
%include "boot/32bit-gdt.asm"
%include "boot/32bit-print.asm"
%include "boot/32bit-switch.asm"

[bits 16]
load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print
    call print_nl
                                        ; we read 2 sectors from bootdisk and save it to [KERNEL_OFFSET]
    mov bx, KERNEL_OFFSET               ; ES:BX - pointer to buffer
    mov dh, 45                   ; number of sectors to read
    mov dl, [BOOT_DRIVE]                ; bootdisk
    call disk_load
    ret


[GLOBAL start] 

[bits 32]
start:
BEGIN_PM:
    mov ebx, MSG_32B_PM
    call print_string_pm
    call KERNEL_OFFSET
    
    jmp $
    
[EXTERN end]

MSG_16B_RM db "Started in 16 bit real mode", 0
MSG_32B_PM db "Loaded 32 bit protected mode", 0
MSG_LOAD_KERNEL db "Loading kernel into memory", 0
MSG_RETURNED_KERNEL db "Returned from kernel", 0
BOOT_DRIVE db 0

times 510-($-$$) db 0
dw 0xaa55