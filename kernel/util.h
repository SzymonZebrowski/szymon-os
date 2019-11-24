#ifndef UTIL_H
#define UTIL_H
#include "../cpu/clock.h"

void memory_copy(char *source, char *dest, int nbytes);
void int_to_str(int n, char str[]);
void introduce();
void print_clock(clock_t *clk);
#endif