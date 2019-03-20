#include "defs.h"

void clear_screen() {
    // Set the screen to blank spaces
    for (int i = 0; i < SCREENWIDTH * SCREENHEIGHT; i++) {
        (screen + i)->Char.UnicodeChar = L' ';
        (screen + i)->Attributes       = 0x07;
    }
}

void draw_char(int x, int y, wchar_t character, unsigned char colour) {
    (screen + x + (y * SCREENWIDTH))->Char.UnicodeChar = character;
    (screen + x + (y * SCREENWIDTH))->Attributes       = colour;
}

void draw_chars(int x, int y, const wchar_t *chars, int len, unsigned char colour) {
    if (len == 0)
        for (int i = 0;; i++) {
            if (*(chars + i) == L'\0') break;
            else draw_char(x + i, y, *(chars + i), colour);
        }
    else
        for (int i = 0; i < len; i++) {
            if (*(chars + i) == L'\0') break;
            else draw_char(x + i, y, *(chars + i), colour);
        }
}

void draw_char_line(int x, int y, wchar_t character, int len, unsigned char colour, int direction) {
    if (direction == DIR_H)
        for (int i = 0; i < len; i++)
            draw_char(x + i, y, character, colour);
    
    else
        for (int i = 0; i < len; i++)
            draw_char(x, y + i, character, colour);
}

// Changes the colours without changing the text
void change_colours(int x, int y, int len, unsigned char colour, int direction) {
    if (direction == DIR_H)
        for (int i = 0; i < len; i++)
            (screen + x + i + (y * SCREENWIDTH))->Attributes = colour;
    
    else
        for (int i = 0; i < len; i++)
            (screen + x + ((y + i) * SCREENWIDTH))->Attributes = colour;
}
