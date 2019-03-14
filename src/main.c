#include <windows.h>
#include "defs.h"

HANDLE    stdin;
HANDLE    console;
CHAR_INFO *screen;



int main() {
    FreeConsole();
    AllocConsole();

    stdin = GetStdHandle(STD_INPUT_HANDLE);
    console = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
                                        0,
                                        NULL,
                                        CONSOLE_TEXTMODE_BUFFER,
                                        NULL);

    screen = calloc(SCREENWIDTH * SCREENHEIGHT, sizeof(CHAR_INFO));

    SetConsoleWindowInfo(console, 1, &smallSMALLRECT);
    SetConsoleScreenBufferSize(console, COORDsize);
    SetConsoleActiveScreenBuffer(console);
    SetConsoleWindowInfo(console, TRUE, &SMALLRECTsize);
    SetConsoleMode(console, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
    SetConsoleMode(stdin, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
    SetConsoleCursorInfo(console, &CURSORINFO); 
}

// Main loop in here
void prog() {
    event_handler()
}
