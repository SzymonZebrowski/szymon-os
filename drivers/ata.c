#include "ata.h"
#include "ports.h"
#include "screen.h"
#include "../libc/string.h"
#include "../libc/util.h"



//status
#define ATA_SR_BSY     0x80    // Busy
#define ATA_SR_DRDY    0x40    // Drive ready
#define ATA_SR_DF      0x20    // Drive write fault
#define ATA_SR_DSC     0x10    // Drive seek complete
#define ATA_SR_DRQ     0x08    // Data request ready
#define ATA_SR_CORR    0x04    // Corrected data
#define ATA_SR_IDX     0x02    // Index
#define ATA_SR_ERR     0x01    // Error

//errors
#define ATA_ER_BBK      0x80    // Bad block
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // Media changed
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // Media change request
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark

//commands
#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

//-----------------------------------------------
#define IDE_ATA        0x00
#define IDE_ATAPI      0x01
 
#define ATA_MASTER     0x00
#define ATA_SLAVE      0x01

#define ATA_PRIMARY      0x00
#define ATA_SECONDARY    0x01
//-----------------------------------------------

//registers
#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

struct IDEChannelRegisters {
    unsigned short base;  // I/O Base.
    unsigned short ctrl;  // Control Base
    unsigned short bmide; // Bus Master IDE
    unsigned char  nIEN;  // nIEN (No Interrupt);
} channels[2];

uint8_t ide_buf[2048] = {0};
static uint8_t ide_irq_invoked = 0;

struct ide_device {
    unsigned char  reserved;    // 0 (Empty) or 1 (This Drive really exists).
    unsigned char  channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
    unsigned char  drive;       // 0 (Master Drive) or 1 (Slave Drive).
    unsigned short type;        // 0: ATA, 1:ATAPI.
    unsigned short signature;   // Drive Signature
    unsigned short capabilities;// Features.
    unsigned int   commandSets; // Command Sets Supported.
    unsigned int   size;        // Size in Sectors.
    unsigned char  model[41];   // Model in string.
} ide_devices[4];

uint8_t ide_read(unsigned char channel, unsigned char reg) {
    unsigned char result;
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    if (reg < 0x08)
        result = port_byte_in(channels[channel].base + reg - 0x00);
    else if (reg < 0x0C)
        result = port_byte_in(channels[channel].base  + reg - 0x06);
    else if (reg < 0x0E)
        result = port_byte_in(channels[channel].ctrl  + reg - 0x0A);
    else if (reg < 0x16)
        result = port_byte_in(channels[channel].bmide + reg - 0x0E);
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    return result;
}

uint8_t ide_read_word(unsigned char channel, unsigned char reg) {
    unsigned char result;
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    if (reg < 0x08)
        result = port_word_in(channels[channel].base + reg - 0x00);
    else if (reg < 0x0C)
        result = port_word_in(channels[channel].base  + reg - 0x06);
    else if (reg < 0x0E)
        result = port_word_in(channels[channel].ctrl  + reg - 0x0A);
    else if (reg < 0x16)
        result = port_word_in(channels[channel].bmide + reg - 0x0E);
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
    return result;
}

void ide_write(unsigned char channel, unsigned char reg, unsigned char data) {
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
    if (reg < 0x08)
        port_byte_out(channels[channel].base  + reg - 0x00, data);
    else if (reg < 0x0C)
        port_byte_out(channels[channel].base  + reg - 0x06, data);
    else if (reg < 0x0E)
        port_byte_out(channels[channel].ctrl  + reg - 0x0A, data);
    else if (reg < 0x16)
        port_byte_out(channels[channel].bmide + reg - 0x0E, data);
    if (reg > 0x07 && reg < 0x0C)
        ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}

uint8_t ide_polling(uint8_t channel, uint32_t advanced_check) {
    
    // (I) Delay 400 nanosecond for BSY to be set:
    // -------------------------------------------------
    for(int i = 0; i < 4; i++)
        ide_read(channel, ATA_REG_ALTSTATUS); // Reading the Alternate Status port wastes 100ns; loop four times.
    
    // (II) Wait for BSY to be cleared:
    // -------------------------------------------------
    while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY)
        ; // Wait for BSY to be zero.
    
    if (advanced_check) {
        uint8_t state = ide_read(channel, ATA_REG_STATUS); // Read Status Register.
    
        // (III) Check For Errors:
        // -------------------------------------------------
        if (state & ATA_SR_ERR)
            //return 2; // Error.
    
        // (IV) Check If Device fault:
        // -------------------------------------------------
        if (state & ATA_SR_DF)
            return 1; // Device Fault.
    
        // (V) Check DRQ:
        // -------------------------------------------------
        // BSY = 0; DF = 0; ERR = 0 so we should check for DRQ now.
        if ((state & ATA_SR_DRQ) == 0)
            return 3; // DRQ should be set
    
    }
    
    return 0; // No Error.

}

unsigned char ide_print_error(unsigned int drive, unsigned char err) {
    if (err == 0)
        return err;
    
    kprint("IDE:\n     ", color_mode(BLACK,WHITE));
    if (err == 1) {kprint("- Device Fault\n     ",color_mode(BLACK,WHITE)); err = 19;}
    else if (err == 2) {
        unsigned char st = ide_read(ide_devices[drive].channel, ATA_REG_ERROR);
        if (st & ATA_ER_AMNF)   {kprint("- No Address Mark Found\n     ",color_mode(BLACK,WHITE));   err = 7;}
        if (st & ATA_ER_TK0NF)   {kprint("- No Media or Media Error\n     ",color_mode(BLACK,WHITE));   err = 3;}
        if (st & ATA_ER_ABRT)   {kprint("- Command Aborted\n     ",color_mode(BLACK,WHITE));      err = 20;}
        if (st & ATA_ER_MCR)   {kprint("- No Media or Media Error\n     ",color_mode(BLACK,WHITE));   err = 3;}
        if (st & ATA_ER_IDNF)   {kprint("- ID mark not Found\n     ",color_mode(BLACK,WHITE));      err = 21;}
        if (st & ATA_ER_MC)   {kprint("- No Media or Media Error\n     ",color_mode(BLACK,WHITE));   err = 3;}
        if (st & ATA_ER_UNC)   {kprint("- Uncorrectable Data Error\n     ",color_mode(BLACK,WHITE));   err = 22;}
        if (st & ATA_ER_BBK)   {kprint("- Bad Sectors\n     ",color_mode(BLACK,WHITE));       err = 13;}
    } else  if (err == 3)           {kprint("- Reads Nothing\n     ",color_mode(BLACK,WHITE)); err = 23;}
    else  if (err == 4)  {kprint("- Write Protected\n     ",color_mode(BLACK,WHITE)); err = 8;}
    kprint("- [",color_mode(BLACK,WHITE));
    kprint((const char *[]){"Primary", "Secondary"}[ide_devices[drive].channel],color_mode(BLACK,WHITE)); // Use the channel as an index into the array
    kprint((const char *[]){"Master", "Slave"}[ide_devices[drive].drive],color_mode(BLACK,WHITE)); // Same as above, using the drive
    kprint("] ",color_mode(BLACK,WHITE));   
    kprint(ide_devices[drive].model,color_mode(BLACK,WHITE));
    kprint("\n",color_mode(BLACK,WHITE));
    
    return err;
}


void ide_initialize(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3,
uint32_t BAR4) {
 
   int i, j, k, count = 0;
 
    // 1- Detect I/O Ports which interface IDE Controller:
    channels[ATA_PRIMARY  ].base  = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
    channels[ATA_PRIMARY  ].ctrl  = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
    channels[ATA_SECONDARY].base  = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
    channels[ATA_SECONDARY].ctrl  = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
    channels[ATA_PRIMARY  ].bmide = (BAR4 & 0xFFFFFFFC) + 0; // Bus Master IDE
    channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE

    // 2- Disable IRQs
    ide_write(ATA_PRIMARY  , ATA_REG_CONTROL, 2);
    ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);

     // 3- Detect ATA-ATAPI Devices:
   for (i = 0; i < 2; i++)
      for (j = 0; j < 2; j++) {
 
         unsigned char err = 0, type = IDE_ATA, status;
         ide_devices[count].reserved = 0; // Assuming that no drive here.
 
         // (I) Select Drive:
         ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive.
         sleep(1); // Wait 1ms for drive select to work.

         // (II) Send ATA Identify Command:
         ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
         sleep(1); // This function should be implemented in your OS. which waits for 1 ms.
                   // it is based on System Timer Device Driver.

         // (III) Polling:
         if (ide_read(i, ATA_REG_STATUS) == 0) continue; // If Status = 0, No Device.
 
         while(1) {
            status = ide_read(i, ATA_REG_STATUS);
            if ((status & ATA_SR_ERR)) {err = 1; break;} // If Err, Device is not ATA.
            if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break; // Everything is right.
         }
 
         // (IV) Probe for ATAPI Devices:
 
         if (err != 0) {
            unsigned char cl = ide_read(i, ATA_REG_LBA1);
            unsigned char ch = ide_read(i, ATA_REG_LBA2);
 
            if (cl == 0x14 && ch ==0xEB)
               type = IDE_ATAPI;
            else if (cl == 0x69 && ch == 0x96)
               type = IDE_ATAPI;
            else
               continue; // Unknown Type (may not be a device).
 
            ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
            sleep(1);

         }
 
         // (V) Read Identification Space of the Device:
        for(int l = 0; l<256; l++)
		{
			*(uint16_t *)(ide_buf + l*2) = ide_read_word(i, ATA_REG_DATA);
		} 
         // (VI) Read Device Parameters:
         ide_devices[count].reserved     = 1;
         ide_devices[count].type         = type;
         ide_devices[count].channel      = i;
         ide_devices[count].drive        = j;
         ide_devices[count].signature    = *((uint16_t *)(ide_buf + ATA_IDENT_DEVICETYPE));
         ide_devices[count].capabilities = *((uint16_t *)(ide_buf + ATA_IDENT_CAPABILITIES));
         ide_devices[count].commandSets  = *((uint32_t *)(ide_buf + ATA_IDENT_COMMANDSETS));
 
         // (VII) Get Size:
         if (ide_devices[count].commandSets & (1 << 26))
            // Device uses 48-Bit Addressing:
            ide_devices[count].size   = *((uint32_t *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
         else
            // Device uses CHS or 28-bit Addressing:
            ide_devices[count].size   = *((uint32_t *)(ide_buf + ATA_IDENT_MAX_LBA));
 
         // (VIII) String indicates model of device (like Western Digital HDD and SONY DVD-RW...):
        for(k = 0; k < 40; k += 2) {
            ide_devices[count].model[k+1] = ide_buf[ATA_IDENT_MODEL + k + 1];
            ide_devices[count].model[k] = ide_buf[ATA_IDENT_MODEL + k];
        }
         ide_devices[count].model[40] = '\0'; // Terminate String.
 
         count++;
      }
}

void ide_print_summary(){
    int i;
    char str[256];
    for (i = 0; i < 4; i++)
        if (ide_devices[i].reserved == 1) {
            kprint("Found ",color_mode(BLACK,WHITE));
            kprint((const char *[]){"ATA", "ATAPI"}[ide_devices[i].type],color_mode(BLACK,WHITE));
            kprint(" Drive ",color_mode(BLACK,WHITE));
            int_to_str(ide_devices[i].size/2/1024,str);
            kprint(str,color_mode(BLACK,WHITE));
            kprint("MB - ", color_mode(BLACK,WHITE));
            kprint(ide_devices[i].model, color_mode(BLACK,WHITE));
            kprint("\n",color_mode(BLACK,WHITE));   
        }
    /*
    uint8_t readdd[1024];
    uint8_t drive = 0;
    //1236725760
    uint8_t x = ide_ata_access(0,drive,0,1,readdd);
    ide_print_error(drive, x);
    print_memory(readdd, 512);
    */
}

void read_from_disk(uint8_t drive, uint8_t start_sector, uint8_t n_sectors, uint8_t data[]){
    uint8_t x= ide_ata_access(0,drive,start_sector, n_sectors, data);
    ide_print_error(drive, x);
    print_memory(data, 512*n_sectors);
    kprint("\n", color_mode(BLACK,WHITE));
}

uint8_t ide_ata_access(uint8_t direction, uint8_t drive, uint32_t lba, uint32_t numsects, uint16_t * data) {
    unsigned char lba_mode /* 0: CHS, 1:LBA28, 2: LBA48 */, dma /* 0: No DMA, 1: DMA */, cmd;
    unsigned char lba_io[6];
    unsigned int  channel      = ide_devices[drive].channel; // Read the Channel.
    unsigned int  slavebit      = ide_devices[drive].drive; // Read the Drive [Master/Slave]
    unsigned int  bus = channels[channel].base; // Bus Base, like 0x1F0 which is also data port.
    unsigned int  words      = 256; // Almost every ATA drive has a sector-size of 512-byte.
    unsigned short cyl, i;
    unsigned char head, sect;
    unsigned char err = 0;

    ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = (ide_irq_invoked = 0x0) + 0x02);



    // (I) Select one from LBA28, LBA48 or CHS;
    if (lba >= 0x10000000) { // Sure Drive should support LBA in this case, or you are
                                // giving a wrong LBA.
        // LBA48:
        lba_mode  = 2;
        lba_io[0] = (lba & 0x000000FF) >> 0;
        lba_io[1] = (lba & 0x0000FF00) >> 8;
        lba_io[2] = (lba & 0x00FF0000) >> 16;
        lba_io[3] = (lba & 0xFF000000) >> 24;
        lba_io[4] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
        lba_io[5] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
        head      = 0; // Lower 4-bits of HDDEVSEL are not used here.
    } else if (ide_devices[drive].capabilities & 0x200)  { // Drive supports LBA?
        // LBA28:
        lba_mode  = 1;
        lba_io[0] = (lba & 0x00000FF) >> 0;
        lba_io[1] = (lba & 0x000FF00) >> 8;
        lba_io[2] = (lba & 0x0FF0000) >> 16;
        lba_io[3] = 0; // These Registers are not used here.
        lba_io[4] = 0; // These Registers are not used here.
        lba_io[5] = 0; // These Registers are not used here.
        head      = (lba & 0xF000000) >> 24;
    } else {
        // CHS:
        lba_mode  = 0;
        sect      = (lba % 63) + 1;
        cyl       = (lba + 1  - sect) / (16 * 63);
        lba_io[0] = sect;
        lba_io[1] = (cyl >> 0) & 0xFF;
        lba_io[2] = (cyl >> 8) & 0xFF;
        lba_io[3] = 0;
        lba_io[4] = 0;
        lba_io[5] = 0;
        head      = (lba + 1  - sect) % (16 * 63) / (63); // Head number is written to HDDEVSEL lower 4-bits.
    }

    // (III) Wait if the drive is busy;
    while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY);

    if (lba_mode == 0)
      ide_write(channel, ATA_REG_HDDEVSEL, 0xA0 | (slavebit << 4) | head); // Drive & CHS.
    else
      ide_write(channel, ATA_REG_HDDEVSEL, 0xE0 | (slavebit << 4) | head); // Drive & LBA

       // (V) Write Parameters;
   if (lba_mode == 2) {
      ide_write(channel, ATA_REG_SECCOUNT1,   0);
      ide_write(channel, ATA_REG_LBA3,   lba_io[3]);
      ide_write(channel, ATA_REG_LBA4,   lba_io[4]);
      ide_write(channel, ATA_REG_LBA5,   lba_io[5]);
   }
   ide_write(channel, ATA_REG_SECCOUNT0,   numsects);
   ide_write(channel, ATA_REG_LBA0,   lba_io[0]);
   ide_write(channel, ATA_REG_LBA1,   lba_io[1]);
   ide_write(channel, ATA_REG_LBA2,   lba_io[2]);

    if (lba_mode == 0 && direction == ATA_READ) cmd = ATA_CMD_READ_PIO;
    if (lba_mode == 1 && direction == ATA_READ) cmd = ATA_CMD_READ_PIO;   
    if (lba_mode == 2 && direction == ATA_READ) cmd = ATA_CMD_READ_PIO_EXT;

    if (lba_mode == 0 && direction == ATA_WRITE) cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 1 && direction == ATA_WRITE) cmd = ATA_CMD_WRITE_PIO;
    if (lba_mode == 2 && direction == ATA_WRITE) cmd = ATA_CMD_WRITE_PIO_EXT;
    ide_write(channel, ATA_REG_COMMAND, cmd);
    
    uint16_t xxx;
    uint8_t str[256];
    if (direction == 0)
         // PIO Read.
    for (i = 0; i < numsects; i++) {
       if (err = ide_polling(channel, 1))
            return err; // Polling, set error and exit if there is.
         for(int z=0; z<words; z++){
            xxx = port_word_in(bus);

            data[z]=xxx;
         }
      } else {
      // PIO Write.
         for (i = 0; i < numsects; i++) {
             for(int z=0; z<words; z++)
                port_word_out(bus, *(data+z));
         }
         ide_write(channel, ATA_REG_COMMAND, (char []) {   ATA_CMD_CACHE_FLUSH,
                        ATA_CMD_CACHE_FLUSH,
                        ATA_CMD_CACHE_FLUSH_EXT}[lba_mode]);
         ide_polling(channel, 0); // Polling.
      }
 
   return 0;
}