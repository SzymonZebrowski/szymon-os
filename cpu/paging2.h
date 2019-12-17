#include "types.h"
#include "isr.h"
#include "../drivers/screen.h"

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));

void *phys_addr(void *vrt_addr);

void page_fault2(registers_t regs);

void init_paging2();


