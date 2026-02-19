#include "terminal.h"
#include "terminal_commande_manageur.h"

#define max_commande_lenght 500

unsigned char commande[max_commande_lenght] ="";
static int current_screen_pos_x = 0;
static int current_screen_pos_y = 0;
static int maxs_creen_size_x = 80;
static int maxs_creens_ize_y = 25;
unsigned char* VGA_screen = (unsigned char*) 0xb8000;
int commande_index =0;

void start_termianl(void){
    print_str("Welcome to N_os\n");
}

void print_char(char character){
    if (character=='\n'){
        current_screen_pos_x = 0;
        current_screen_pos_y++;
    }
    else if(character=='\b'){
        commande[commande_index]= '\0';
        execute_commande(commande);
        commande_index = 0;
        commande[0] = '\n';
    }
    else{
        int position = (current_screen_pos_y * maxs_creen_size_x + current_screen_pos_x) * 2;
        VGA_screen[position] = character;
        VGA_screen[position + 1] = 0x0f;

        commande[commande_index] = character;
        commande_index++;

        current_screen_pos_x++;
        
        if (current_screen_pos_x == maxs_creen_size_x){
            maxs_creen_size_x = 0;
            current_screen_pos_y++;
            
            if (current_screen_pos_y == maxs_creens_ize_y){
                current_screen_pos_y = 0;
            }
        }
    }
}

void clear_terminal(void){
    for (int i = 0; i < maxs_creen_size_x * maxs_creens_ize_y * 2; i += 2){
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