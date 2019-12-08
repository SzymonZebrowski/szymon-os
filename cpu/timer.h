 
#ifndef TIMER_H
#define TIMER_H

#include "types.h"

void init_timer(u32 freq);
void pit_timeout_set(u32 val);
void pit_timeout_unset();
void get_time();
u8 pit_timeout_reached();
void sleep(u32 ms);

#endif