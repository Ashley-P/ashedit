#include "defs.h"
#include "input.h"
#include "text.h"

HANDLE    h_stdin;
HANDLE    console;
CHAR_INFO *screen;
int isRunning;
struct Buffer *command_line; // Special buffer where we just don't use some of the members
enum ControlState control_state;

void console_init() {
    FreeConsole();
    AllocConsole();

    h_stdin = GetStdHandle(STD_INPUT_HANDLE);
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
    SetConsoleMode(h_stdin, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT);
    SetConsoleCursorInfo(console, &CURSORINFO); 
}

// Main loop in here
int main() {
    console_init();

    // Some other initialisation
    control_state = CS_EDIT;
    isRunning = 1;

    init_lists();

    // Setting up the command line
    command_line = init_command_line();

    // Setting up the very first buffer and window
    struct Buffer *buf = init_buffer(0, 0);
    init_window(buf, 0); // The second arg doesn't matter


    while (isRunning) {
        redraw_screen();
        event_handler(control_state);
    }
}
