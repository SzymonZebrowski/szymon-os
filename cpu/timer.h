 
#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void init_timer(uint32_t freq);
void pit_timeout_set(uint32_t val);
void pit_timeout_unset();
void get_time();
uint8_t pit_timeout_reached();
void sleep(uint32_t ms);

#endif