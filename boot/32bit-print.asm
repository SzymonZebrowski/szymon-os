[bits 32] ; 32-bit protected mode

VIDEO_MEMORY equ 0xb8000    ; address of VGA memory
ORANGE_ON_BLACK equ 0x0c     ; orange color

print_string_pm:
    pusha
    mov edx, VIDEO_MEMORY   

print_string_pm_loop:
    mov al, [ebx]           ; ebx is address of character
    mov ah, ORANGE_ON_BLACK

    cmp al, 0               ; end of string
    je print_string_pm_done

    mov [edx], ax
    add ebx, 1              ; next character
    add edx, 2              ; next position in VGA memory

    jmp print_string_pm_loop

print_string_pm_done:
    popa
    ret