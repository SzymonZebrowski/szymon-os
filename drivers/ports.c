#include "ports.h"

u8 port_byte_in(u16 port){
    u8 result;
    asm("in %%dx, %%al" : "=a" (result) : "d" (port));
    //byte from port in dx goes to al register

    return result;

}

void port_byte_out(u16 port, u8 data){
    asm("out %%al, %%dx" : : "a" (data), "d" (port));
}

u8 port_word_in(u16 port){
    u8 result;   

    asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
    //word from port in dx goes to al register

     return result;
}

void port_word_out(u16 port, u16 data){
    asm("out %%ax, %%dx" : : "a" (data), "d" (port));
}