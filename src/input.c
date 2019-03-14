#include <windows.h>
#include "defs.h"

// Prototype
void handle_keys(KEY_EVENT_RECORD kev, enum ControlState state);


void event_handler(enum ControlState state) {
    unsigned long evread;
    INPUT_RECORD inpbuf[256];

    ReadConsoleInput(stdin,
                     inpbuf,
                     256,
                     &evread);

    for(int i = 0; i < evread; i++) {
        switch (inpbuf[i].EventType) {
            case KEY_EVENT:
                /* Pass the key event to handle_keys where it gets interpreted based on the program state */
                handle_keys(inpbuf[i].Event.KeyEvent, state);

            case MOUSE_EVENT: case WINDOW_BUFFER_SIZE_EVENT: case FOCUS_EVENT: case MENU_EVENT:
                // Ignore these
                break;
        }
    }
}

void handle_keys(KEY_EVENT_RECORD kev, enum ControlState state) {
}
