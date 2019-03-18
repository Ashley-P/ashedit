#include <windows.h>
#include "commands.h"
#include "defs.h"
#include "text.h"
#include "utils.h"

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
        struct Buffer *buf = get_active_buffer();
        switch (kev.wVirtualKeyCode) {
            case 0x30 ... 0x39: // Numbers [0-9]
            case 0x41 ... 0x5A: // Letters [a-z]
            case 0xBA ... 0xC0: // Other
            case 0xDB ... 0xDF: // Other
            case VK_SPACE:
            case 0xE2:          // Backslash
                // @NOTE : Implement Line wrapping
                // @FIXME
                insert_char(buf, buf->curs_x, buf->curs_y, kev.uChar.UnicodeChar);
                buf->curs_x++;
                return;
        }
    }

    // Edit Mode specific things
    if (state == CS_EDIT) {
        struct Buffer *buf = get_active_buffer();
        switch (kev.wVirtualKeyCode) {
            case VK_BACK:
                if (buf->curs_x == 0 && buf->curs_y == 0) return;
                else if (buf->curs_x == 0) {
                    buf->curs_x = w_string_len(*(buf->ch_array + buf->curs_y - 1));
                    w_string_cat(*(buf->ch_array + buf->curs_y), *(buf->ch_array + buf->curs_y - 1),
                            buf->x_len_max, buf->x_len_max);
                    delete_line(buf, buf->curs_y);
                    buf->curs_y--;
                } else {
                    delete_char(buf, buf->curs_x - 1, buf->curs_y); 
                    buf->curs_x--;
                }
                return;
            case VK_RETURN:
                insert_line(buf, buf->curs_y);
                w_string_split(*(buf->ch_array + buf->curs_y), *(buf->ch_array + buf->curs_y + 1),
                        buf->x_len_max, buf->x_len_max, buf->curs_x);
                buf->curs_x = 0;
                buf->curs_y++;
                return;
            case VK_LEFT:
                if (buf->curs_x == 0 && buf->curs_y == 0) return;
                else if (buf->curs_x == 0) {
                    buf->curs_y--;
                    buf->curs_x = w_string_len(*(buf->ch_array + buf->curs_y));
                } else
                    buf->curs_x--;
                return;
            case VK_RIGHT:
                if (buf->curs_x == w_string_len(*(buf->ch_array + buf->curs_y))
                        && buf->curs_y == buf->y_len - 1) return;
                else if (buf->curs_x == w_string_len(*(buf->ch_array + buf->curs_y))) {
                    buf->curs_x = 0;
                    buf->curs_y++;
                } else
                    buf->curs_x++;
                return;
            case VK_UP:
                if (buf->curs_y == 0) return;
                if (buf->curs_x > w_string_len(*(buf->ch_array + buf->curs_y - 1)))
                    buf->curs_x = w_string_len(*(buf->ch_array + buf->curs_y - 1));
                buf->curs_y--;
                return;
            case VK_DOWN:
                if (buf->curs_y == buf->y_len - 1) return;
                if (buf->curs_x > w_string_len(*(buf->ch_array + buf->curs_y + 1)))
                    buf->curs_x = w_string_len(*(buf->ch_array + buf->curs_y + 1));
                buf->curs_y++;
                return;
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
