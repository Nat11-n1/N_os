void print_str(char *text){
    int position_x = 0;
    for (int i = 0; text[i] != '\0'; i++){
        unsigned short entry = (0x07 << 8) | text[i];
        *(unsigned short*)(0xB8000 + (position_x) * 2) = entry;
        position_x ++;
    }
}

void _start(){
    print_str("Hello ,world!");
    while(1){};
}