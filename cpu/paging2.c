#include "paging2.h"

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));


void fill_pd(){
    for(int i=0; i<1024; i++){
        page_directory[i] = 0x00000002; //kernel, rw, not present
    }
}

void fill_pt(){
    for(int i=0; i<1024; i++){
        first_page_table[i]=(i*0x1000) | 3; //kernel level, rw, present
    }
}


void *phys_addr(void *vrt_addr){
    // 10 bit pd_index, 10 bit pd_index, 12 bit offset
    uint32_t pd_index = (uint32_t) vrt_addr >> 22;    // first 10 bits
    uint32_t pt_index = (uint32_t) vrt_addr >> 12 & 0x3ff; //middle ten bits
    uint32_t offset = (uint32_t) vrt_addr & 0x00000fff;

    uint32_t *pd = (uint32_t)0xfffff000;
    uint32_t *pt = (uint32_t)0xFFC00000 + 0x400*pd_index;

    return (void*)((pt[pt_index] & ~0xfff)) + offset;
}

void page_fault2(registers_t regs){
    uint32_t faulting_address;
    asm("mov %%cr2, %0": "=r" (faulting_address));

    int present = !(regs.err_code & 0x1); //page not present
    int rw = regs.err_code & 0x2;           // Write operation?
    int us = regs.err_code & 0x4;           // Processor was in user-mode?
    int reserved = regs.err_code & 0x8;     // Overwritten CPU-reserved bits of page entry?
    int id = regs.err_code & 0x10;          // Caused by an instruction fetch?

    kprint("Page fault (", color_mode(BLACK|WHITE));
    if(present) kprint("present ", color_mode(BLACK|WHITE));
    if(rw) kprint("read-only ", color_mode(BLACK|WHITE));
    if(us) kprint("user-mode ", color_mode(BLACK|WHITE));
    if(reserved) kprint("reserved ", color_mode(BLACK|WHITE));
    kprint(") at", color_mode(BLACK|WHITE));
    char str[256];
    int_to_hex_str(faulting_address, str);
    kprint(str, color_mode(BLACK|WHITE));
    kprint("\n",color_mode(BLACK|WHITE));

    asm volatile("cli");
    while(1);
    kprint("xd",color_mode(BLACK|BRIGHT_BLUE));
}

void init_paging2(){
    register_interrupt_handler(14, page_fault2);

    fill_pd();
    fill_pt();
    page_directory[0] = ((uint32_t)first_page_table) | 3;

    //fill_pt();
    //page_directory[0] = (uint32_t)page_tables | 3;

    asm volatile("mov %0, %%cr3" :: "r"(&page_directory));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000001;  //oldest bit enables paging;
    asm volatile ("mov %0, %%cr0" :: "r" (cr0));

}