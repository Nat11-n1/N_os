#include "kernel\lib\types.h"

#ifndef fat32_h
#define fat32_h

void fat32_init();
void fat32_read_cluster(uint32_t cluster, uint8_t* buffer);
uint32_t fat32_next_cluster(uint32_t cluster);
void fat32_list_dir(uint32_t cluster);
void fat32_read_file(uint32_t dir_cluster, char* filename, uint8_t* buffer);

#endif