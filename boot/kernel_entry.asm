global start
[bits 32]

start:
    [extern kmain]
    call kmain
    jmp $