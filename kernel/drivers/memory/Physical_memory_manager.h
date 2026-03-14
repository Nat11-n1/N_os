#include "kernel\lib\types.h"

#ifndef fat32_h
#define fat32_h

void PMM_init(void);
uint64_t PMM_alloc_page(void);
int PMM_free_page(uint64_t value);

#endif