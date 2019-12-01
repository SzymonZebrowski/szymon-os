#include "kernel.h"
#include "../libc/util.h"
#include "../libc/string.h"
#include "../drivers/screen.h"
#include "../drivers/keyboard.h"
#include "../cpu/isr.h"
#include "../cpu/idt.h"
#include "../cpu/timer.h"
#include "../cpu/clock.h"
#include "../cpu/paging.h"


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
    //kprint(input,color_mode(BLACK,WHITE));
    kprint("> ",color_mode(BLACK,WHITE));
}

void main(){
    clear_screen();
    introduce();
    kprint("> ",color_mode(BLACK,WHITE));

    isr_install();
    irq_install();

    //init_paging();

    int *xx =  (int*)0xa00000000;
}