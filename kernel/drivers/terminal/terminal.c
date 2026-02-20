#include "terminal.h"
#include "terminal_commande_manageur.h"
#include "lib\hex_utils.h"

#define max_commande_lenght 500

unsigned char commande[max_commande_lenght];
int current_screen_pos_x = 0;
int current_screen_pos_y = 0;
static int max_screen_size_x = 80;
static int max_screen_size_y = 25;
unsigned char* VGA_screen = (unsigned char*) 0xb8000;
int commande_index =0;
unsigned char screenchar[25][80];

void start_terminal(void){
    //initialise the terminal
    for (int y = 0; y < max_screen_size_y; y++) {
        for (int x = 0; x < max_screen_size_x; x++) {
            screenchar[y][x] = ' ';
        }
    }
    print_str("Welcome to N_os");
    return_to_line();
}

void print_char(char character){
    //execute the commande
    if(character=='\n'){
        commande[commande_index]= '\0';
        execute_commande(commande);
        commande_index = 0;
        commande[0] = '\0';
        return_to_line();
    }
    //delete last character
    else if(character=='\b'){
        if (commande_index !=0){
            commande_index = commande_index-1;
            commande[commande_index]='\0';
        }
        int lastposition = (current_screen_pos_y * max_screen_size_x + current_screen_pos_x-1) * 2;
        VGA_screen[lastposition] = ' ';
        VGA_screen[lastposition + 1] = 0x0f;
        screenchar[current_screen_pos_y][current_screen_pos_x-1] = ' ';
        if (current_screen_pos_x != 0){
            current_screen_pos_x--;
        }else{
            if(current_screen_pos_y!=0){
                current_screen_pos_x=max_screen_size_x-1;
                current_screen_pos_y--;
            }
            else{
                current_screen_pos_x =0;
            }
        }
    }
    //print the character
    else{
        int position = (current_screen_pos_y * max_screen_size_x + current_screen_pos_x) * 2;
        VGA_screen[position] = character;
        VGA_screen[position + 1] = 0x0f;

        commande[commande_index] = character;
        commande_index++;

        screenchar[current_screen_pos_y][current_screen_pos_x] = character;

        current_screen_pos_x++;
        
        if (current_screen_pos_x == max_screen_size_x){
        current_screen_pos_x = 0;
        current_screen_pos_y++;
            
        if (current_screen_pos_y == max_screen_size_y){
            scroll_screen();
        }
    }
    }
}

void clear_terminal(void){
    for (int i = 0; i < max_screen_size_x * max_screen_size_y * 2; i += 2){
        VGA_screen[i] = ' ';
        VGA_screen[i + 1] = 0x0f;
    }
    current_screen_pos_x = 0;
    current_screen_pos_y = 0;
}

void print_str(unsigned char *string){
    for(int i = 0; string[i] != '\0'; i++){
        print_char(string[i]);
    }
}

void scroll_screen(void) {
    for (int y = 0; y < max_screen_size_y - 1; y++) {
        for (int x = 0; x < max_screen_size_x; x++) {
            screenchar[y][x] = screenchar[y+1][x];

            int dst_pos = (y * max_screen_size_x + x) * 2;
            VGA_screen[dst_pos]     = screenchar[y][x];
            VGA_screen[dst_pos + 1] = 0x0f;
        }
    }

    int last_y = max_screen_size_y - 1;
    for (int x = 0; x < max_screen_size_x; x++) {
        screenchar[last_y][x] = ' ';

        int pos = (last_y * max_screen_size_x + x) * 2;
        VGA_screen[pos]     = ' ';
        VGA_screen[pos + 1] = 0x0f;
    }
}

void return_to_line(void) {
    current_screen_pos_x = 0;

    if (current_screen_pos_y < max_screen_size_y - 1) {
        current_screen_pos_y++;
    } else {
        scroll_screen();
        current_screen_pos_y = max_screen_size_y - 1;
    }
}

void update_cursor(void){
    int position = (current_screen_pos_y * max_screen_size_x + current_screen_pos_x) * 2;
    char position_hexa = to_hex(position);
    inb(0x3d4,14);
    outb(0x3d5);
    inb(0x3d4,15);
    outb(0x3d5);
}