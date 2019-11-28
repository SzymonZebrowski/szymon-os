#ifndef UTIL_H
#define UTIL_H
#include "../cpu/clock.h"
#include "../libc/string.h"

void introduce();
void print_clock(clock_t *clk);
void time(clock_t *clk);

#endif