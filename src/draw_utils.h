#ifndef DRAW_UTILS_H
#define DRAW_UTILS_H

#include "defs.h"


void clear_screen();
void draw_character(int x, int y, wchar_t character, unsigned char colour);
void draw_characters(int x, int y, wchar_t *characters, unsigned char colour); // Needs null terminated string
void draw_character_line(int x, int y, wchar_t character, int len, unsigned char colour);


#endif
