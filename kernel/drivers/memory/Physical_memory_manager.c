#include "Physical_memory_manager.h"
#include "kernel\lib\types.h"

extern uint64_t _kernel_end;
uint64_t RAM_Total = 0;
uint8_t *bitmap = (uint8_t *)&_kernel_end;
uint64_t total_pages;
uint64_t bitmap_size;
struct entryE820
{
    uint64_t base;
    uint64_t size;
    uint32_t type;
};

void PMM_init(void){
    uint16_t *entry_count = (uint16_t *)0x5FF8;
    struct entryE820 *map = (struct entryE820 *)0x6000;
    for (int i =0;i!=*entry_count ;i++){
        if (RAM_Total <(map[i].base  +map[i].size)){
            RAM_Total = map[i].base  +map[i].size;
        }
    }
    total_pages = RAM_Total/4096;
    bitmap_size = (total_pages + 7)/8;
    for (int i =0; i<bitmap_size;i++){
        bitmap[i] = 0xFF;
    }
    for (int i =0;i!=*entry_count ;i++){
        if (map[i].type == 1){
            uint64_t first_page = map[i].base/4096;
            uint64_t nbr_pages = map[i].size/4096;
            for (uint64_t current_page = first_page;current_page<nbr_pages+first_page;current_page++){
                bitmap[current_page / 8] &= ~(1 << (current_page % 8));
            }
        }
    }
    //protect the bios and kernel in the ram
    for (uint64_t current = 0x0;current<0x100000/4096;current++){
        bitmap[current / 8] |= (1 << (current % 8));
    }
    for (uint64_t current = 0x10000/4096;current < (uint64_t)&_kernel_end/4096;current++){
        bitmap[current / 8] |= (1 << (current % 8));
    }
    for (uint64_t current = (uint64_t)&_kernel_end/4096;current<(uint64_t)&_kernel_end/4096+(bitmap_size+4095)/4096;current++){
        bitmap[current / 8] |= (1 << (current % 8));
    }
}

uint64_t PMM_alloc_page(void){
    for (uint64_t page = 0;page <total_pages;page++){
        if (!(bitmap[page/8] & (1 << (page%8)))){
            bitmap[page/8] |= (1 << (page%8));
            return page*4096;
        }
    }
    return 0xFFFFFFFFFFFFFFFF;
}

int PMM_free_page(uint64_t value){
    if (value % 4096 == 0){
        uint64_t page = value / 4096;
        bitmap[page/8] &= ~(1 << (page%8));
        return 1;
    }
    else{
    return 0;
    }
}