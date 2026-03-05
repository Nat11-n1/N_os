#include "app_manageur.h"

extern char task[10000][64];

void start_task(unsigned char* task_name){
    for(int i;i<10000-1;i++){
        if (task[i][0]=='\0'){
            task[i][0] = task_name;
            break;
        }
    }
}

void close_task(int task){

}