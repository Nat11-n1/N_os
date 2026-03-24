#include "terminal_commande_manageur.h"
#include "kernel\lib\strcmp.h"
#include "kernel\drivers\terminal\terminal.h"
#include "kernel\drivers\storage\filesysteme\filesysteme.h"

#define maxmaincommande 50
#define maxobjectcommande 450

static unsigned char maincommande[maxmaincommande];
static unsigned char objectcommande[maxobjectcommande];

void execute_commande(unsigned char *commande){
    int i = 0;
    int j = 0;
    maincommande[0] = '\0';
    objectcommande[0] = '\0';

    while (commande[i]==' '){
        i++;
    }

    for (j = 0; commande[i] != '\0' && j < maxmaincommande-1;i++){
        if (commande[i]!=' '){
            maincommande[j] = commande[i];
            j++;
        }
        else{
            while (commande[i]==' '){
                i++;
            }
            maincommande[j]='\0';
            break;
        }
    }
    for (j = 0; commande[i] != '\0'&& j < maxobjectcommande-1;j++,i++){
            objectcommande[j] = commande[i];
    }
    objectcommande[j] = '\0';
    find_commande(maincommande);
}

void find_commande(unsigned char* commande){
    if (issamestr(commande,"clear")){
        clear_terminal();
    }
    else if(issamestr(commande,"dir")){
        void list_files();
    }
    else if(issamestr(commande,"exe")){
        void exe_file(objectcommande);
    }
    else if(issamestr(commande,"ls")){
        list_files();
    }
    else if(issamestr(commande,"pwd")){
        print_str(current_dir,0);
    }
    else{
        print_str(commande,0);
        print_str(" : unknown command !",1);
    }
}