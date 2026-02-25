#include "ata.h"

int diskready = 0;

void ata_init() {
    outb(0x1F6, 0xA0);
    outb(0x1F7, 0xEC);
    if (inb(0x1F7) == 0){
        return; // No drive found
    }else{
        ata_wait(); // Drive found
    }
}

void ata_wait() {
    while (diskready==0){
        uint8_t status = inb(0x1F7);
        if ((status & 0x80) == 0 && (status & 0x40) != 0){
            diskready = 1; //disk is ready
        }
    }
}

void ata_read_sector(uint32_t lba, uint8_t* buffer) {
    if (diskready==1){
        outb(0x1F2, 1);
        outb(0x1F3, lba & 0xFF);
        outb(0x1F4, (lba >> 8) & 0xFF);
        outb(0x1F5, (lba >>16)& 0xFF);
        outb(0x1F6, 0xE0 | ((lba>>24) & 0x0F));
        outb(0x1F7,0x20);
        ata_wait();
        for (int i =0;i<256;i++){
            ((uint16_t*)buffer)[i] = inw(0x1F0);
        }
    }
}

void ata_write_sector(uint32_t lba, uint8_t* buffer) {
    if (diskready==1){
        outb(0x1F2, 1);
        outb(0x1F3, lba & 0xFF);
        outb(0x1F4, (lba >> 8) & 0xFF);
        outb(0x1F5, (lba >>16)& 0xFF);
        outb(0x1F6, 0xE0 | ((lba>>24) & 0x0F));
        outb(0x1F7,0x30);
        ata_wait();
        for (int i =0;i<256;i++){
            outw(0x1F0, ((uint16_t*)buffer)[i]);
        }
    }
}