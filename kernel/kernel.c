#include "drivers\terminal\terminal.h"
#include "drivers\storage\ata_pio\ata.h"

void _start(void){
    ata_init();
    start_terminal();
    while(1);
}