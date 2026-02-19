#include "strcmp.h"

int i= 0;

int issamestr(unsigned char* str1,unsigned char* str2){
    for (i=0; str1[i]!='\0' && str2[i]!='\0'; i++){
        if (str1[i] != str2[i]){
            return 0;
        }
    }
    if (str1[i] == str2[i]){
        return 1;
    }
    return 0;
}