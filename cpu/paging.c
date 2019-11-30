#include "paging.h"

u32 *frames;
u32 nframes;

//page directories
page_directory_t *kernel_directory=0;
page_directory_t *current_directory=0;

extern u32 placement_address;

// using bitmap for storing information about presence of frame-
//1 bit is enough to give necessary information;

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

// function to set a bit in the frames bitset
static void set_frame(u32 frame_addr){
   u32 frame = frame_addr/0x1000;
   u32 idx = INDEX_FROM_BIT(frame);
   u32 off = OFFSET_FROM_BIT(frame);
   frames[idx] |= (0x1 << off);
}

// function to clear a bit in the frames bitset
static void clear_frame(u32 frame_addr){
   u32 frame = frame_addr/0x1000;
   u32 idx = INDEX_FROM_BIT(frame);
   u32 off = OFFSET_FROM_BIT(frame);
   frames[idx] &= ~(0x1 << off);
}

// function to test if a bit is set.
static u32 test_frame(u32 frame_addr){
   u32 frame = frame_addr/0x1000;
   u32 idx = INDEX_FROM_BIT(frame);
   u32 off = OFFSET_FROM_BIT(frame);
   return (frames[idx] & (0x1 << off));
}

// Static function to find the first free frame.
static u32 first_frame(){
   u32 i, j;
   for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
   {
       if (frames[i] != 0xFFFFFFFF) // nothing free;
       {
           // at least one bit is free here.
           for (j = 0; j < 32; j++)
           {
               u32 toTest = 0x1 << j;
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
        u32 idx = first_frame();
        if(idx == (u32)-1){
            kprint("No free frames!", color_mode(BLACK, WHITE));
        }
        set_frame(idx*0x1000);
        page->present = 1;  //frame is loaded
        page->rw = is_writeable? 1:0;
        page->user = is_kernel? 0:1;
        page->frame=idx;
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

void initialise_paging(){
    u32 mem_end_page = 0x40000000;  // assume that physical memory is 1GB
    nframes = mem_end_page / 0x1000;
    frames = (u32*)kmalloc(INDEX_FROM_BIT(nframes));
    memory_set(frames, 0, INDEX_FROM_BIT(nframes));

    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    memory_set(kernel_directory,0,sizeof(page_directory_t));
    current_directory = kernel_directory;

    int i=0;
    while(i < placement_address){
        alloc_frame(get_page(i, 1, kernel_directory), 0,0);
        i += 0x1000;
    }

    register_interrupt_handler(14, page_fault);
    switch_page_directory(kernel_directory);
}

void switch_page_directory(page_directory_t* new_pd){
    current_directory = new_pd;
    asm("mov %0, %%cr3" :: "r"(&new_pd->tablesPhysical));
    u32 cr0;
    asm("mov %%cr0, %0": "=r"(cr0));
    cr0 |= 0x80000000;  //oldest bit enables paging;
    asm("mov %0, %%cr0":: "r"(cr0));
}

page_t *get_page(u32 address, int make, page_directory_t *dir){
    u32 idx = address/0x1000;   //page number
    u32 table_idx = idx / 1024; // idx of page entry containing this address

    if(dir->tables[table_idx]){ //if akready assigned
        return &dir->tables[table_idx]->pages[idx%1024];
    }
    else if(make){
        u32 tmp;
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
    u32 faulting_address;
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
    char str[256] = int_to_str_hex(faulting_address);
    kprint(str, color_mode(BLACK|WHITE));
    kprint("\n",color_mode(BLACK|WHITE));

    asm("jmp $");
    kprint("xd",color_mode(BLACK|BRIGHT_BLUE));
}