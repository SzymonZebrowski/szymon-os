#ifndef MEM_H
#define MEM_H

#include "../cpu/types.h"

void memory_copy(u8 *source, u8 *dest, u32 nbytes);
void memory_set(u8 *dest, u8 val, u32 len);

u32 kmalloc_a(u32 sz);  // page aligned.
u32 kmalloc(u32 sz); // normal.
u32 kmalloc_ap(u32 sz, u32 *phys);

#endif
