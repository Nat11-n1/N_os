#include "terminal.h"

static int currentscreensizex = 0;
static int currentscreensizey = 0;
static int maxscreensizex = 80;
static int maxscreensizey = 25;
unsigned char* VGA_screen = (unsigned char*) 0xb8000;

void print_char(char character){
    if (character=='\n'){
        currentscreensizex = 0;
        currentscreensizey++;
    }
    else if(character=='\b'){
        
    }
    else{
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
}

void clear_terminal(void){
    for (int i = 0; i < maxscreensizex * maxscreensizey * 2; i += 2){
        VGA_screen[i] = ' ';
        VGA_screen[i + 1] = 0x0f;
    }
    currentscreensizex = 0;
    currentscreensizey = 0;
}

void print_str(unsigned char *string){
    for(int i = 0; string[i] != '\0'; i++){
        print_char(string[i]);
    }
}