#include "kernel/drivers/terminal/terminal.h"
#include "keybord/keybord.h"
#include "inputmanageur.h"

void keyinkeybordpressed(char key){
    print_char(key,1);
};

void enable_user_input(void){
    iskeypressed();
}