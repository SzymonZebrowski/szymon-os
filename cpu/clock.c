#include "clock.h"
#include "../drivers/ports.h"
#include "../drivers/screen.h"
#include "../libc/util.h"

#define CURRENT_YEAR 2019
 
uint32_t century_register = 0x00;   

enum {
      cmos_address = 0x70,
      cmos_data    = 0x71
};

 
static uint32_t get_update_in_progress_flag() {
      port_byte_out(cmos_address, 0x0A);
      return (port_byte_in(cmos_data) & 0x80);
}
 
static uint8_t get_RTC_register(uint32_t reg) {
      port_byte_out(cmos_address, reg);
      return port_byte_in(cmos_data);
}

uint32_t read_rtc() {
    clock_t clk;
    uint8_t registerB;
    uint8_t last_second, last_minute, last_hour;
    uint8_t last_day, last_month, last_year;
    uint8_t last_century;
 
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

    return (uint32_t) &clk;
}