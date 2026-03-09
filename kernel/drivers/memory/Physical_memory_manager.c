#include "Physical_memory_manager.h"
#include "kernel\lib\types.h"

extern uint64_t _kernel_end;
uint64_t RAM_Total = 0;
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
    uint8_t *bitmap = (uint8_t *)&_kernel_end;
    uint64_t total_pages = RAM_Total/4096;
    uint64_t bitmap_size = (RAM_Total+7)/8;
    for (int i =0; i=bitmap_size-1;i++){
        map[i].size = 0xFF;
    }
     for (int i =0;i!=*entry_count ;i++){
        if (map[i].type = 1){
            
        }
    }
}