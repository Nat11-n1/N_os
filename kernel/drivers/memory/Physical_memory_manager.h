#ifndef Physical_memory_manager_h
#define Physical_memory_manager_h

#include "kernel\lib\types.h"

void PMM_init(void);
uint64_t PMM_alloc_page(void);
int PMM_free_page(uint64_t value);

#endif