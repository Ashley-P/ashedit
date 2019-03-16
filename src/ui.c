#include "defs.h"
#include "draw_utils.h"


void draw_ui() {
    // Bottom row is for typing commands so it's left blank

#if 1
    // Some test that will get removed at some point
    draw_char_line(0, 0, L'A', 10, 0x89, DIR_H);
#endif
}

void redraw_screen() {
    clear_screen();
    draw_ui();

    WriteConsoleOutputW(console,
                        screen,
                        COORDsize,
                        (COORD) {0, 0},
                        &SMALLRECTsize);
}
