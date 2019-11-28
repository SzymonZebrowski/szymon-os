#include "timer.h"
#include "../drivers/screen.h"
#include "../libc/util.h"
#include "clock.h"
#include "isr.h"

u32 tick = 0;
static u32 pit_timeout_value = 0;

void pit_timeout_set(u32 val){
    pit_timeout_value = val;
}

void pit_timeout_unset() {
    pit_timeout_value = 0;
}

u8 pit_timeout_reached(){
    return !(pit_timeout_value>0);
}

static void timer_callback(registers_t regs) {
    clock_t* clk = (clock_t*)read_rtc();

    print_clock(clk);

    if(pit_timeout_value){
        pit_timeout_value--;
    }
}

void init_timer(u32 freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    u32 divisor = 1193180 / freq;
    u8 low  = (u8)(divisor & 0xFF);
    u8 high = (u8)( (divisor >> 8) & 0xFF);
    /* Send the command */
    port_byte_out(0x43, 0x36); /* Command port */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}