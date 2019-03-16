#ifndef DRAW_UTILS_H
#define DRAW_UTILS_H


void clear_screen();
void draw_char(int x, int y, wchar_t character, unsigned char colour);
void draw_chars(int x, int y, const wchar_t *chars, unsigned char colour); // Needs null terminated string
void draw_char_line(int x, int y, wchar_t character, int len, unsigned char colour, int direction);
void change_colours(int x, int y, int len, unsigned char colour, int direction);


#endif
