#include <windows.h>
#include "commands.h"
#include "defs.h"
#include "text.h"

// Prototype
void handle_keys(KEY_EVENT_RECORD kev, enum ControlState state);


void event_handler(enum ControlState state) {
    unsigned long evread;
    INPUT_RECORD inpbuf[256];

    ReadConsoleInput(h_stdin,
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
        switch (kev.wVirtualKeyCode) {
            case 0x30 ... 0x39: // Numbers [0-9]
            case 0x41 ... 0x5A: // Letters [a-z]
            case 0xBA ... 0xC0: // Other
            case 0xDB ... 0xDF: // Other
            case VK_SPACE:
            case 0xE2:          // Backslash
                insert_char(get_active_buffer(), kev.uChar.UnicodeChar);
                return;
        }
    }

    // Edit Mode specific things
    if (state == CS_EDIT) {
        switch (kev.wVirtualKeyCode) {
            case VK_BACK:
                delete_char(get_active_buffer());
                return;
            case VK_RETURN:
                new_line(get_active_buffer());
                return;
            case VK_LEFT:
            case VK_RIGHT: return;
            case VK_UP: return;
            case VK_DOWN: return;
        }
    }

    // Command Mode specific things
    if (state == CS_COMMAND) {
        switch (kev.wVirtualKeyCode) {
            case VK_BACK: return;
            case VK_RETURN: return;
            case VK_LEFT: return;
            case VK_RIGHT: return;
            case VK_UP: return;
            case VK_DOWN: return;
        }
    }

    // Other stuff
    switch (kev.wVirtualKeyCode) {
        case VK_ESCAPE: isRunning = 0; return;
        default: return;
    }
}
