#include "kernel\drivers\terminal\terminal.h"
#include "kernel\drivers\userinput\keybord\keybord.h"

void keyinkeybordpressed(char key){
    print_char(key);
};

void enable_user_input(void){
    iskeypressed();
}