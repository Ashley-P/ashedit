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
    // Immediate return if it's not a press
    if (!kev.bKeyDown) return;

    // If statements allow for fall through
    // Shared Presses
    if (state & (CS_EDIT | CS_COMMAND)) {
    }

    // Edit Mode specific things
    if (state == CS_EDIT) {
    }

    // Command Mode specific things
    if (state == CS_COMMAND) {
    }

    // Other stuff
    switch (kev.wVirtualKeyCode) {
        case VK_ESCAPE: isRunning = 0; return;
        default: return;
    }
}
