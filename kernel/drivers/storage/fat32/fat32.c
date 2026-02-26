#include "ata_pio\ata.h"

int bytes_per_sector=0;
int sectors_per_cluster=0;
int reserved_sectors=0;
int fat_count=0;
int sectors_per_fat=0;
int root_cluster=0;

int fat_start=0;
int data_start=0;

void fat32_init(){
    uint8_t buffer[512];
    ata_read_sector(0, buffer);
    bytes_per_sector = buffer[0x0B] | (buffer[0x0C] << 8);
    sectors_per_cluster = buffer[0x0D];
    reserved_sectors = buffer[0x0E] | (buffer[0x0F] << 8);
    fat_count = buffer[0x10];
    sectors_per_fat = buffer[0x24] | (buffer[0x25] << 8) | (buffer[0x26] << 16) | (buffer[0x27] << 24);
    root_cluster = buffer[0x2C] | (buffer[0x2D] << 8) | (buffer[0x2E] << 16) | (buffer[0x2F] << 24);

    fat_start  = reserved_sectors;
    data_start = reserved_sectors + (fat_count * sectors_per_fat);
}