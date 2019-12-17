#ifndef ATA_H
#define ATA_H

#include "../cpu/types.h"

#define      ATA_READ      0x00
#define      ATA_WRITE     0x01

void ide_initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4);
uint8_t ide_ata_access(uint8_t direction, uint8_t drive, uint32_t lba, uint32_t numsects, uint16_t * data);
void ide_print_summary();
void read_from_disk(uint8_t drive, uint8_t start_sector, uint8_t n_sectors, uint8_t data[]);
#endif