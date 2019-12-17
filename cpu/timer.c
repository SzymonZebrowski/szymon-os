#include "timer.h"
#include "../drivers/screen.h"
#include "../libc/util.h"
#include "clock.h"
#include "isr.h"

uint32_t tick = 0;
static uint32_t divisor;
static volatile uint32_t sleeping = 0;
static uint32_t pit_timeout_value = 0;

void pit_timeout_set(uint32_t val){
    pit_timeout_value = val;
}

void pit_timeout_unset() {
    pit_timeout_value = 0;
}

uint8_t pit_timeout_reached(){
    return !(pit_timeout_value>0);
}

static void timer_callback(registers_t regs) {
    clock_t* clk = (clock_t*)read_rtc();

    print_clock(clk);

    if(sleeping) sleeping--;
    if(pit_timeout_value){
        pit_timeout_value--;
    }
}

void get_time(){
    clock_t* clk = (clock_t*)read_rtc();
    
    time(clk);
}

void init_timer(uint32_t freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    divisor = 1193180 / freq;
    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)( (divisor >> 8) & 0xFF);
    /* Send the command */
    port_byte_out(0x43, 0x36); /* Command port */
    port_byte_out(0x40, low);
    port_byte_out(0x40, high);
}

void sleep(uint32_t ms){
    sleeping = (ms/((float)10000/divisor))+1;
    while(sleeping);
}