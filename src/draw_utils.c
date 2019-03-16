#include "defs.h"

void clear_screen() {
    // Set the screen to blank spaces
    for (int i = 0; i < SCREENWIDTH * SCREENHEIGHT; i++) {
        (screen + i)->Char.UnicodeChar = L' ';
        (screen + i)->Attributes       = 0x07;
    }
}
