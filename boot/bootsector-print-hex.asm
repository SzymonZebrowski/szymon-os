RESULT:
    db '0x0000',0

print_hex:              ; printing 0-9 and a-f
    pusha

    mov cx, 0           ; counter

; ASCII codes: '0'-'9' -> 0x30 - 0x39
;              'A'-'F' -> 0x41 - 0x46

loop:
    cmp cx, 4
    je hex_end

    mov ax, dx          ; dx contains letters
    and ax, 0x000f      ; get the last character
    
    add al, 0x30
    cmp al, 0x39        ; code of '9'
    jle move_to_mem
    add al, 0x7         ; if last four bits were A, this makes 'A' ASCII value

move_to_mem:
    mov bx, RESULT + 5  ; address of last zero in '0x0000'
    sub bx, cx          ; get current bit of RESULT
    mov [bx], al        ; move actual ASCII to memory
    ror dx, 4           ; shift register for next letters

    inc cx
    jmp loop

hex_end:

    mov bx, RESULT  ; print function require address in bx
    call print

    popa
    ret

    


