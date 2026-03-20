#ifndef utils_h
#define utils_h

#include "types.h"

static inline void halt(void){
    __asm__ volatile("cli");
    __asm__ volatile("hlt");
}

static inline void invlpg(uint64_t addr){
    __asm__ volatile("invlpg (%0)" :: "r"(addr) : "memory");
}

#endif