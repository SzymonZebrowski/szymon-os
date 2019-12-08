#ifndef ATA_H
#define ATA_H

#include "../cpu/types.h"

#define      ATA_READ      0x00
#define      ATA_WRITE     0x01

void ide_initialize(u32 BAR0, u32 BAR1, u32 BAR2, u32 BAR3, u32 BAR4);
u8 ide_ata_access(u8 direction, u8 drive, u32 lba, u32 numsects, u16 * data);
void ide_print_summary();

#endif