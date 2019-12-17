#include "ports.h"

uint8_t port_byte_in(uint16_t port){
    uint8_t result;
    asm("in %%dx, %%al" : "=a" (result) : "d" (port));
    //byte from port in dx goes to al register

    return result;

}

void port_byte_out(uint16_t port, uint8_t data){
    asm("out %%al, %%dx" : : "a" (data), "d" (port));
}

uint16_t port_word_in(uint16_t port){
    uint16_t result;   

    asm("in %%dx, %%ax" : "=a" (result) : "d" (port));
    //word from port in dx goes to al register

    return result;
}

void port_word_out(uint16_t port, uint16_t data){
    asm("out %%ax, %%dx" : : "a" (data), "d" (port));
}
