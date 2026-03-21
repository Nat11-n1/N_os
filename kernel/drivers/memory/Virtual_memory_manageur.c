#include "Virtual_memory_manageur.h"
#include "kernel\lib\types.h"
#include "Physical_memory_manager.h"
#include "kernel\lib\utils.h"



int VMM_map_page(uint64_t virtual_address,uint64_t physical_address){
    uint64_t CR3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(CR3));

    uint64_t *pml4 = (uint64_t *)(CR3 & ~0xFFF);
    uint64_t pml4_index = (virtual_address >> 39) & 0x1FF;

    // check if pml4 is valid
    if (!(pml4[pml4_index] & 1)){
        uint64_t *new_page = PMM_alloc_page();
        for (int i = 0; i < 512; i++){
            new_page[i] = 0;
        }
        pml4[pml4_index] = (uint64_t)new_page | 0x03;
    }

    uint64_t *PDPT = (uint64_t *)(pml4[pml4_index] & ~0xFFF);
    uint64_t PDPT_index = (virtual_address >> 30) & 0x1FF;

    //check if PDPT is valid
    if (!(PDPT[PDPT_index] & 1)){
        uint64_t *new_page = PMM_alloc_page();
        for (int i = 0; i < 512; i++){
            new_page[i] = 0;
        }
        PDPT[PDPT_index] = (uint64_t)new_page | 0x03;
    }

    uint64_t *PD = (uint64_t *)(PDPT[PDPT_index] & ~0xFFF);
    uint64_t PD_index = (virtual_address >> 21) & 0x1FF;

    //check if PD is valid
    if (!(PD[PD_index] & 1)){
        uint64_t *new_page = PMM_alloc_page();
        for (int i = 0; i < 512; i++){
            new_page[i] = 0;
        }
        PD[PD_index] = (uint64_t)new_page | 0x03;
    }

    uint64_t *PT = (uint64_t *)(PD[PD_index] & ~0xFFF);
    uint64_t PT_index = (virtual_address >> 12) & 0x1FF;

    //write PT
    PT[PT_index] = physical_address | 0x03;
    return 1;
}

void VMM_unmap_page(uint64_t virtual_address){
    //go to PT
    uint64_t CR3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(CR3));
    uint64_t *pml4 = (uint64_t *)(CR3 & ~0xFFF);
    uint64_t pml4_index = (virtual_address >> 39) & 0x1FF;
    uint64_t *PDPT = (uint64_t *)(pml4[pml4_index] & ~0xFFF);
    uint64_t PDPT_index = (virtual_address >> 30) & 0x1FF;
    uint64_t *PD = (uint64_t *)(PDPT[PDPT_index] & ~0xFFF);
    uint64_t PD_index = (virtual_address >> 21) & 0x1FF;
    uint64_t *PT = (uint64_t *)(PD[PD_index] & ~0xFFF);
    uint64_t PT_index = (virtual_address >> 12) & 0x1FF;

    //invalidate the virtual_address
    PT[PT_index] = 0;
    invlpg(virtual_address);
}