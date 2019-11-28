#include "clock.h"
#include "../drivers/ports.h"
#include "../drivers/screen.h"
#include "../libc/util.h"

#define CURRENT_YEAR 2019
 
u32 century_register = 0x00;   

enum {
      cmos_address = 0x70,
      cmos_data    = 0x71
};

 
static u32 get_update_in_progress_flag() {
      port_byte_out(cmos_address, 0x0A);
      return (port_byte_in(cmos_data) & 0x80);
}
 
static u8 get_RTC_register(u32 reg) {
      port_byte_out(cmos_address, reg);
      return port_byte_in(cmos_data);
}

u32 read_rtc() {
    clock_t clk;
    u8 registerB;
    u8 last_second, last_minute, last_hour;
    u8 last_day, last_month, last_year;
    u8 last_century;
 
      // Note: This uses the "read registers until you get the same values twice in a row" technique
      //       to avoid getting dodgy/inconsistent values due to RTC updates
 
    while (get_update_in_progress_flag());                // Make sure an update isn't in progress
    clk.second = get_RTC_register(0x00);
    clk.minute = get_RTC_register(0x02);
    clk.hour = get_RTC_register(0x04);
    clk.day = get_RTC_register(0x07);
    clk.month = get_RTC_register(0x08);
    clk.year = get_RTC_register(0x09);
    clk.century = clk.year / 100;

 
    do {
          last_second = clk.second;
          last_minute = clk.minute;
          last_hour = clk.hour;
          last_day = clk.day;
          last_month = clk.month;
          last_year = clk.year;
          last_century = clk.century;
   
          while (get_update_in_progress_flag());           // Make sure an update isn't in progress
          clk.second = get_RTC_register(0x00);
          clk.minute = get_RTC_register(0x02);
          clk.hour = get_RTC_register(0x04);
          clk.day = get_RTC_register(0x07);
          clk.month = get_RTC_register(0x08);
          clk.year = get_RTC_register(0x09);
    
    } while(((last_second != clk.second) || (last_minute != clk.minute) || (last_hour != clk.hour) ||
             (last_day != clk.day) || (last_month != clk.month) || (last_year != clk.year) ||
             (last_century != clk.century) ));
      registerB = get_RTC_register(0x0B);
     
    // Convert BCD to binary values if necessary

    if (!(registerB & 0x04)) {
        clk.second = (clk.second & 0x0F) + ((clk.second / 16) * 10);
        clk.minute = (clk.minute & 0x0F) + ((clk.minute / 16) * 10);
        clk.hour = ( (clk.hour & 0x0F) + (((clk.hour & 0x70) / 16) * 10) ) | (clk.hour & 0x80);
        clk.hour++;
        clk.day = (clk.day & 0x0F) + ((clk.day / 16) * 10);
        clk.month = (clk.month & 0x0F) + ((clk.month / 16) * 10);
        clk.year = (clk.year & 0x0F) + ((clk.year / 16) * 10);
        
    }
 
    // Convert 12 hour clock to 24 hour clock if necessary

    if (!(registerB & 0x02) && (clk.hour & 0x80)) {
          clk.hour = ((clk.hour & 0x7F) + 12) % 24;
    }

    return (u32) &clk;
}