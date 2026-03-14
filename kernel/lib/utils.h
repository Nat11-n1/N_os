#ifndef utils_h
#define utils_h

static inline void halt(void){
    __asm__ volatile("cli");
    __asm__ volatile("hlt");
}

#endif