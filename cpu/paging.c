#include "paging.h"

uint32_t *frames;
uint32_t nframes;

//page directories
page_directory_t *kernel_directory=0;
page_directory_t *current_directory=0;

extern uint32_t placement_address;
extern void enable_paging();
// using bitmap for storing information about presence of frame-
//1 bit is enough to give necessary information;

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

// function to set a bit in the frames bitset
static void set_frame(uint32_t frame_addr){
   uint32_t frame = frame_addr/0x1000;
   uint32_t idx = INDEX_FROM_BIT(frame);
   uint32_t off = OFFSET_FROM_BIT(frame);
   frames[idx] |= (0x1 << off);
}

// function to clear a bit in the frames bitset
static void clear_frame(uint32_t frame_addr){
   uint32_t frame = frame_addr/0x1000;
   uint32_t idx = INDEX_FROM_BIT(frame);
   uint32_t off = OFFSET_FROM_BIT(frame);
   frames[idx] &= ~(0x1 << off);
}

// function to test if a bit is set.
static uint32_t test_frame(uint32_t frame_addr){
   uint32_t frame = frame_addr/0x1000;
   uint32_t idx = INDEX_FROM_BIT(frame);
   uint32_t off = OFFSET_FROM_BIT(frame);
   return (frames[idx] & (0x1 << off));
}

// Static function to find the first free frame.
static uint32_t first_frame(){
   uint32_t i, j;
   for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
   {
       if (frames[i] != 0xFFFFFFFF) // nothing free;
       {
           // at least one bit is free here.
           for (j = 0; j < 32; j++)
           {
               uint32_t toTest = 0x1 << j;
               if ( !(frames[i]&toTest) )
               {
                   return i*32+j;
               }
           }
       }
   }
}

void alloc_frame(page_t *page, int is_kernel, int is_writeable){
    if(page->frame != 0){
        return;
    }
    else{
        uint32_t idx = first_frame();
        if(idx == (uint32_t)-1){
            kprint("No free frames!", color_mode(BLACK, WHITE));
        }
        set_frame(idx*0x1000);
        page->present = 1;  //frame is loaded
        page->rw = is_writeable? 1:0;
        page->user = is_kernel? 0:1;
        page->frame=idx;
        page->unused |= 0x10;
    }
}

void free_frame(page_t *page){
    if(!page->frame){
        return;
    }else{
        clear_frame(page->frame);   //free page
        page->frame = 0x0;
    }
}

void init_paging(){
    uint32_t mem_end_page = 0x1000000;  // assume that physical memory is 1GB
    
    nframes = mem_end_page / 0x1000;
    frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
    memory_set(frames, 0, INDEX_FROM_BIT(nframes));

    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    memory_set(kernel_directory,0,sizeof(page_directory_t));
    //current_directory = kernel_directory;
    kernel_directory->physicalAddr = (uint32_t) kernel_directory->tablesPhysical;

    int i=0;
    page_t *p;
    while(i < placement_address){
        p = get_page(i, 1, kernel_directory);
        alloc_frame(p, 0,0);
        i += 0x1000;
    }

    register_interrupt_handler(14, page_fault);
    //asm volatile("cli");
    switch_page_directory(kernel_directory);
   // enable_paging(kernel_directory);
    int asjd = 1023;
}

void switch_page_directory(page_directory_t* new_pd){
    current_directory = new_pd;
    asm volatile("mov %0, %%cr3" :: "r"(&new_pd->physicalAddr));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;  //oldest bit enables paging;
    asm volatile ("mov %0, %%cr0" :: "r" (cr0));
}

page_t *get_page(uint32_t address, int make, page_directory_t *dir){
    uint32_t idx = address/0x1000;   //page number
    uint32_t table_idx = idx / 1024; // idx of page entry containing this address

    if(dir->tables[table_idx]){ //if akready assigned
        return &dir->tables[table_idx]->pages[idx%1024];
    }
    else if(make){
        uint32_t tmp;
        dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t),&tmp);
        memory_set(dir->tables[table_idx], 0, 0x1000);
        dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, USER
        return &dir->tables[table_idx]->pages[idx%1024];
    }
    else{
        return 0;
    }
}
void page_fault(registers_t regs){
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

    //asm volatile("cli");
    while(1);
    kprint("xd",color_mode(BLACK|BRIGHT_BLUE));
}