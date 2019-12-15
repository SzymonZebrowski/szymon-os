#include "kernel.h"
#include "../libc/util.h"
#include "../libc/string.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../drivers/ata.h"
#include "../cpu/isr.h"
#include "../cpu/idt.h"
#include "../cpu/timer.h"
#include "../cpu/clock.h"
#include "../cpu/paging2.h"


void user_input(char *input){
    if (strcmp(input, "END") == 0) {
        kprint("Stopping the CPU. Bye!\n",color_mode(BLACK,WHITE));
        asm volatile("hlt");
    }
    else if(strcmp(input, "TIME") == 0){
        get_time();
    }
    else if(strcmp(input, "CLEAR") == 0){
        clear_screen();
        kprint("\n",color_mode(BLACK,WHITE));
    }
    else if(strcmp(input, "OS") == 0){
        clear_screen();
        introduce();
    }
    else if(strcmp(input, "DISK") == 0){
        u8 data[512];
        read_from_disk(0,0,1,data);
    }
    //kprint(input,color_mode(BLACK,WHITE));
    kprint("> ",color_mode(BLACK,WHITE));
}

void main(){
    clear_screen();
    introduce();

    isr_install();
    irq_install();

	ide_initialize(0x1F0, 0x3F6, 0x170, 0x376, 0x000);
    ide_print_summary();

    
    kprint("> ",color_mode(BLACK,WHITE));
    init_paging2();

    kprint("\n> xdddd hellooo",color_mode(BLACK,WHITE));

    int *t = (u32*)0xa00000000;
}