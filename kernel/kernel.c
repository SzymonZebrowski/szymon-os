#include "util.h"
#include "../drivers/screen.h"
#include "../cpu/isr.h"
#include "../cpu/idt.h"

void main(){
    clear_screen();
    introduce();

    /* Fill up the screen */
    int i = 0;
    for (i = 0; i < 23; i++) {
        char str[255];
        int_to_str(i, str);
        kprint_at(str, 0, i, color_mode(BLACK, WHITE));
    }

    clear_screen();
    introduce();

    //kprint_at("This text forces the kernel to scroll. Row 0 will disappear. ", 60, 24, color_mode(BLACK,WHITE));
    kprint("And with this text, the kernel will scroll again, and row 1 will disappear too!\n", color_mode(BLACK,WHITE));
    
    isr_install();
    /* Test the interrupts */
    int j = 255-128;
    int x = 10/0;
    int z = 89+123;
    u8 jj = 5000;

    kprint("\n\nxDD", color_mode(BLACK, WHITE));
}