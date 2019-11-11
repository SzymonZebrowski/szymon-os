gdt_start:

gdt_null:
    dd 0x0
    dd 0x0

gdt_code:
    dw 0xffff       ;limit bits 0-15
    dw 0x0          ;base bits 0-15
    db 0x0          ;base bits 16-23
    db 10011010b    ;flags bits 15-8 (including type flags)
    db 11001111b    ;flags bits 23-16 (including segment limit flags)
    db 0x0          ;base bits 31-24

gdt_data:
    dw 0xffff       ;same as above, difference if one flag
    dw 0x0
    db 0x0
    db 10010010b    ;changed Executable bit (set 0 for data selector)
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start


CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start