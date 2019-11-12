[org 0x7c00]    ;bootloader offset
    mov bp, 0x9000  ;set the stack
    mov sp, bp

    mov bx, MSG_16B_RM
    call print;

    call switch_to_pm;

%include "boot/bootsector-print-hex.asm"
%include "boot/bootsector-print.asm"
%include "boot/bootsector-disk-load.asm"
%include "boot/32bit-gdt.asm"
%include "boot/32bit-print.asm"
%include "boot/32bit-switch.asm"

[bits 32]
BEGIN_PM:
    mov ebx, MSG_32B_PM
    call print_string_pm
    
    jmp $

MSG_16B_RM db "Started in 16 bit real mode\n", 0
MSG_32B_PM db "Loaded 32 bit protected mode\n", 0

times 510-($-$$) db 0
dw 0xaa55