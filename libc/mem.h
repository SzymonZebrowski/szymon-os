#ifndef MEM_H
#define MEM_H

#include "../cpu/types.h"

void memory_copy(uint8_t *source, uint8_t *dest, uint32_t nbytes);
void memory_set(uint8_t *dest, uint8_t val, uint32_t len);

uint32_t kmalloc_a(uint16_t sz);  // page aligned.
uint32_t kmalloc(uint16_t sz); // normal.
uint32_t kmalloc_ap(uint16_t sz, uint32_t *phys);

#endif
