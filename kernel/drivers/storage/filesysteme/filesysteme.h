#ifndef fat32_h
#define fat32_h

typedef unsigned int uint32_t;
extern uint32_t current_dir;

void list_files(void);
void exe_file(char* filename);

#endif