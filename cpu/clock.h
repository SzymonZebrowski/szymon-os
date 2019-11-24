#ifndef CLOCK_H
#define CLOCK_H

#include "types.h"

typedef struct {
    u8 second;
    u8 minute;
    u8 hour;
    u8 day;
    u8 month;
    u8 year;
    u8 century;
} clock_t;

u32 read_rtc();

#endif