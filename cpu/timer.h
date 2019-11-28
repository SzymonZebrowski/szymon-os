 
#ifndef TIMER_H
#define TIMER_H

//#include "../libc/util.h"
#include "types.h"

void init_timer(u32 freq);
void pit_timeout_set(u32 val);
void pit_timeout_unset();
u8 pit_timeout_reached();

#endif