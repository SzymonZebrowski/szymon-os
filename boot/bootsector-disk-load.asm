MSG_DISK_ERROR:
    db "Error during reading disk", 0

disk_load:
    pusha
    push dx

    mov ah, 0x02    ; int 0x13 function, 0x02 means READ
    mov al, dh      ; number of sectors to read (1-128)
    mov cl, 0x02    ; sector's number   (1-17)
    mov ch, 0x0     ; track/cylinder number (0-1023)
    mov dh, 0x00    ; head number (0-15)
    
    ; dl - drive number, passed by caller

    int 0x13
    jc disk_error

    pop dx
    popa
    ret

disk_error:
    mov bx, MSG_DISK_ERROR
    call print
    call print_nl
    mov dh, ah
    call print_hex
    jmp $