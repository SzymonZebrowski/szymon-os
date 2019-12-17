#include "mem.h"

extern uint32_t end;
//uint32_t placement_address = (uint32_t)&end;
uint32_t placement_address = 0x10000;

void memory_copy(uint8_t *source, uint8_t *dest, uint32_t nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(uint8_t *dest, uint8_t val, uint32_t len) {
    uint8_t *temp = (uint8_t *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

uint32_t kmalloc_int(uint16_t s, int align, uint32_t *phys){
    if(align && (placement_address & 0xFFFFF000)){  //if not already aligned
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    if (phys)
    {
        *phys = placement_address;
    }
    uint32_t tmp = placement_address;
    placement_address += s;
    return tmp;
}

uint32_t kmalloc_a(uint16_t s){
    return kmalloc_int(s, 1, 0);
};  // page aligned.

uint32_t kmalloc_ap(uint16_t s, uint32_t *phys)
{
    return kmalloc_int(s, 1, phys);
}

uint32_t kmalloc(uint16_t s){
    return kmalloc_int(s, 0, 0);
}; // normal.
