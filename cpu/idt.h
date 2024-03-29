#ifndef IDT_H
#define IDT_H

#include "types.h"

#define KERNEL_CS 0x08

//definition of interrupt gate
typedef struct {
    u16 low_offset; //lower 16 bits of handler function address
    u16 sel;        // kernel segment selector
    u8 always0;
    /*
    Bit 7: Interupt is present;
    Bits 6-5: Privilege level of caller (0=kernel, .., 3=user)
    Bit 4: Set to 0 for interrupt gates
    Bits 3-0: bits 1110b = 14 = 32 bit interrupt gate
    */
    u8 flags;
    u16 high_offset; //higher 16 bits of handler function address
} __attribute__((packed)) idt_gate_t;


/* A pointer to the array of interrupt handlers.
 * Assembly instruction 'lidt' will read it */
typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) idt_register_t;

#define IDT_ENTRIES 256
idt_gate_t idt[IDT_ENTRIES];
idt_register_t idt_reg;


/* Functions implemented in idt.c */
void set_idt_gate(int n, u32 handler);
void set_idt();

#endif