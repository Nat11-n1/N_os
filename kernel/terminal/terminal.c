#include "terminal.h"

static int currentscreensizey = 0;
static int currentscreensizex = 0;
static int maxscreensizex = 80;
static int maxscreensizey = 25;
static unsigned char* VGA_screen = (unsigned char*) 0xb8000;

void print_char(char character){
    int position = (currentscreensizey * maxscreensizex + currentscreensizex) * 2;
    VGA_screen[position] = character;
    VGA_screen[position + 1] = 0x0f;
    
    currentscreensizex++;
    
    if (currentscreensizex == maxscreensizex){
        currentscreensizex = 0;
        currentscreensizey++;
        
        if (currentscreensizey == maxscreensizey){
            currentscreensizey = 0;
        }
    }
}