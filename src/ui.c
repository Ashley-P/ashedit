#include "defs.h"
#include "draw_utils.h"


void draw_ui() {
    (screen + 1)->Char.UnicodeChar = L'A';
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
