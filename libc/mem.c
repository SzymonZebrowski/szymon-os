#include "mem.h"

extern u32 end;
u32 placement_address = (u32) &end;

void memory_copy(u8 *source, u8 *dest, u32 nbytes) {
    int i;
    for (i = 0; i < nbytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void memory_set(u8 *dest, u8 val, u32 len) {
    u8 *temp = (u8 *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

u32 kmalloc_int(u32 s, int align){
    if(align && (placement_address & 0xFFFFF000)){  //if not already aligned
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    u32 tmp = placement_address;
    placement_address += s;
    return tmp;
}

u32 kmalloc_a(u32 s){
    return kmalloc_int(s, 1);
};  // page aligned.

u32 kmalloc(u32 s){
    return kmalloc_int(s,0);
}; // normal.
