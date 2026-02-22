#include "ata.h"

void ata_init() {
    uint8_t inb(uint16_t port);
    void outb(uint16_t port, uint8_t value);
}

void ata_wait() {
}

void ata_read_sector(uint32_t lba, uint8_t* buffer) {
}

void ata_write_sector(uint32_t lba, uint8_t* buffer) {
}