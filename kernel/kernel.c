#include "drivers\terminal\terminal.h"
#include "Start_management.h"
#include "lib\utils.h"

void _start(void){
    if (start_os() != 1){
        halt();
    }
    start_terminal();
    while(1);
}
