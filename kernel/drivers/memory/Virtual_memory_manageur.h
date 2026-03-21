#ifndef Virtual_memory_manageur_h
#define Virtual_memory_manageur_h

int VMM_map_page(uint64_t virtual_address,uint64_t physical_address);
void VMM_unmap_page(uint64_t virtual_address);

#endif