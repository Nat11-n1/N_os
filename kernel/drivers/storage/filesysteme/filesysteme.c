#include "kernel\drivers\storage\fat32\fat32.h"
#include "kernel\drivers\terminal\terminal.h"

uint32_t current_dir =0;
static uint8_t dir_buffer[128 * 512];

void list_files(void){
    int filesfound =0;
    fat32_list_dir(current_dir, &filesfound);
    print_char(filesfound,0);
    print_str("file(s) found(s) in this directory",0);
    return_to_line();
}

void exe_file(char* filename){
    void fat32_read_file(current_dir, filename, dir_buffer);
    void (*entry)() = (void (*)())dir_buffer;
    entry();
}

void open_file(char* filename){
    void fat32_read_file(current_dir, filename, dir_buffer);
    clear_terminal;
    
}