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
    if (state & CS_ANY) {
        if (kev.dwControlKeyState & LEFT_CTRL_PRESSED) {
            switch (kev.wVirtualKeyCode) {
                case 0x31: // '1'
                    control_state = CS_EDIT;
                    w_string_reset(*(command_line->ch_array), command_line->x_len_max);
                    return;
                case 0x32: // '2'
                    control_state = CS_COMMAND;
                    command_line->curs_x = 0;
                    command_line->curs_y = 0;
                    return;
                case 0x33: // '3'
                    //control_state = CS_EDIT;
                    //w_string_reset(*(command_line->ch_array), command_line->x_len_max);
                    return;
            }
        } else {
            switch (kev.wVirtualKeyCode) {
                case VK_ESCAPE:
                    isRunning = 0;
                    return;
            }
        }
    }

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
                if (state == CS_COMMAND) {
                    if (w_string_len(*command_line->ch_array) == command_line->x_len_max - 1) return;
                    insert_char(command_line, command_line->curs_x, 0, kev.uChar.UnicodeChar);
                    command_line->curs_x++;
                } else {
                    if (w_string_len(*(buf->ch_array + buf->curs_y)) == buf->x_len_max - 1) return;
                    insert_char(buf, buf->curs_x, buf->curs_y, kev.uChar.UnicodeChar);
                    buf->curs_x++;
                }
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
            case VK_BACK:
                if (command_line->curs_x == 0) return;
                else {
                    delete_char(command_line, command_line->curs_x - 1, 0); 
                    command_line->curs_x--;
                }
                return;
            case VK_RETURN:
                // Command intepreter would be called here
                command_line->curs_x = 0;
                command_line->curs_y = 0;
                shift_pointers_right((void **) command_line->ch_array, command_line->y_len_true, 1, 0);
                *command_line->ch_array = calloc(command_line->x_len_max, sizeof(wchar_t));
                if (command_line->y_len != command_line->y_len_true - 1) command_line->y_len++;
                return;
            case VK_LEFT:
                if (command_line->curs_x == 0) return;
                command_line->curs_x--;
                return;
            case VK_RIGHT:
                if (command_line->curs_x == w_string_len(*command_line->ch_array)) return;
                command_line->curs_x++;
                return;
            // Scrolling through previous commands
            case VK_UP:
                if (command_line->curs_y == command_line->y_len - 1) return;
                w_string_reset(*command_line->ch_array, command_line->x_len_max);
                command_line->curs_y++;
                w_string_cpy(*(command_line->ch_array + command_line->curs_y), *command_line->ch_array);
                command_line->curs_x = w_string_len(*(command_line->ch_array + command_line->curs_y));
                return;
            case VK_DOWN:
                if (command_line->curs_y == 0) return;
                else if (command_line->curs_y == 1) {
                    w_string_reset(*command_line->ch_array, command_line->x_len_max);
                    command_line->curs_y--;
                    command_line->curs_x = 0;
                } else {
                    w_string_reset(*command_line->ch_array, command_line->x_len_max);
                    command_line->curs_y--;
                    w_string_cpy(*(command_line->ch_array + command_line->curs_y), *command_line->ch_array);
                    command_line->curs_x = w_string_len(*(command_line->ch_array + command_line->curs_y));
                }
                return;
        }
    }
}
