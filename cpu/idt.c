#include "idt.h"
#include "../kernel/util.h"

void set_idt_gate(int n, u32 handler){
    idt[n].low_offset = low_16(handler);
    idt[n].sel = KERNEL_CS;
    idt[n].always0 = 0;
    idt[n].flags = 0x8E;    
    /*10001110b;  
        b7 - interrupt is present
        b6-5 - kernel level;
        b4 - set to 0 for interrupt gates
        b3-0 - interrupt gate
    */
    idt[n].high_offset = high_16(handler);
}

void set_idt(){
    idt_reg.base = (u32) &idt;
    idt_reg.limit = IDT_ENTRIES * sizeof(idt_gate_t) - 1;

    asm volatile("lidtl (%0)" : : "r" (&idt_reg));
}