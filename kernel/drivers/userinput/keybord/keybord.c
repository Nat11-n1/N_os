#include "kernel/drivers/userinput/inputmanageur.h"
#include "keybord.h"

unsigned char kbd_azerty[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', ')', '=', '\b', /* 0x00 - 0x0E */
    '\t', 'a', 'z', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', '$', '\n',     /* 0x0F - 0x1C */
    0,   /* 0x1D : Ctrl left */
    'q', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', '%', '*',               /* 0x1E - 0x29 */
    0,   /* 0x2A : Shift left */
    '*', 'w', 'x', 'c', 'v', 'b', 'n', ',', ';', ':', '!',                    /* 0x2B - 0x35 */
    0,   /* 0x36 : Shift right */
    '*', 
    0,   /* 0x38 : Alt left */
    ' ', /* 0x39 : Space */
    0,   /* 0x3A : Caps Lock */
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0,        /* 0x3B - 0x44 : F1 to F10 */
    0,   /* 0x45 : Num Lock */
    0,   /* 0x46 : Scroll Lock */
    '7', '8', '9', '-',                  /* 0x47 - 0x4A */
    '4', '5', '6', '+',                  /* 0x4B - 0x4E */
    '1', '2', '3',                       /* 0x4F - 0x51 */
    '0', '.',                            /* 0x52 - 0x53 */
    0, 0, 0,                             /* 0x54 - 0x56 */
    0, 0,                                /* 0x57 - 0x58 : F11 and F12 */
    0
};

unsigned char getkey(void){
    unsigned char scancode = inb(0x60);
    unsigned char ascii = kbd_azerty[scancode];
    return ascii;
}

void iskeypressed(void){
    while((inb(0x64) & 0x01)==0);
    keyinkeybordpressed(getkey);
}