#include "drivers\memory\Physical_memory_manager.h"
#include "drivers\storage\ata_pio\ata.h"
#include "Start_management.h"

int start_os(void){
    PMM_init();     // init the memory (RAM)
    ata_init();     // init the storage
    return 1;
}