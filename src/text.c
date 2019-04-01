/**
 * This file handles anything text specific that isn't already handling in utils.c
 * namely initializing and deinitalising buffers and windows
 * along with handling their usage
 */

#include <stdarg.h>
#include <stdio.h>
#include "defs.h"
#include "draw_utils.h"
#include "utils.h"

// A list of all the buffers and windows
struct Buffer **buffers; 
struct Window **windows;
size_t b_len;
size_t w_len;

// The current active window
struct Window *active_win;

// A string that get's displayed where the command line is without being interactable
wchar_t global_message[MAX_BUFSIZE_SMALL];
unsigned char global_message_colour;
int view_global_message;

/**
 * Checks and modifies text_offset in the active window depending on what curs_y is and the
 * size of the window
 */
static inline void text_offset_check() {
    struct Window *win = active_win;
    struct Buffer *buf = win->buffer;

    if (buf->curs_y < win->text_offset) {
        if (buf->curs_y == 0)
            win->text_offset = 0;
        else
            win->text_offset = buf->curs_y - 1;
    } else if (buf->curs_y > win->text_offset + win->height - 1)
        win->text_offset = buf->curs_y - win->height + 1;
}

/**
 * @NOTE : Implement Line numbering
 * @NOTE : Implement Line wrapping
 */
void draw_ui() {
    // Set the text_offset for the active window
    text_offset_check();


    // Each window has a bottom bar and a left bar
    // There is no left bar if the win->x is 0
    int x_offset;
    // Window drawing
    for (int i = 0; i < w_len; i++) {
        struct Window *win = *(windows + i);
        struct Buffer *buf = win->buffer;
        if (win->x == 0)
            x_offset = 0;
        else {
            // Draw the vertical bar that would seperate the window
            draw_char_line(win->x, win->y, LIGHT_VERTICAL, win->height, 0x70, DIR_V);
            x_offset = 1;
        }

        // Drawing each line of text
        int j;
        for (j = 0; j < win->height; j++) {
            // Start from the offset
            if ((j + win->text_offset) >= buf->y_len) break;
            draw_chars(win->x + x_offset, win->y + j, *(buf->ch_array + win->text_offset + j),
                  win->width - x_offset, 0x07);
        }

        // Draw Tildes on the rest of the window
        for (; j < win->height; j++)
            draw_char(win->x + x_offset, win->y + j, L'~', 0x07);


        // Drawing the bottom border of the window
        change_colours(win->x, win->y + win->height, win->width, 0x70, DIR_H);
        if (w_string_cmp(buf->fn_relative, L"\0"))
            draw_chars(win->x, win->y + win->height, L"New File", win->width - x_offset, 0x70);
        else
            draw_chars(win->x, win->y + win->height, buf->fn_relative, win->width - x_offset, 0x70);
    }

    // If view_global_message != 0 then we draw the message, otherwise we draw the command line
    if (view_global_message == 1) 
        draw_chars(0, SCREENHEIGHT - 1, global_message, SCREENWIDTH - 1, global_message_colour);
    else
        draw_chars(0, SCREENHEIGHT - 1, *(command_line->ch_array), SCREENWIDTH - 1, 0x07);

    // Draw the cursor wherever it's supposed to be
    //if (control_state == CS_EDIT) {
    struct Window *win = active_win;
    struct Buffer *buf = win->buffer;

    if (win->x == 0)
        x_offset = 0;
    else
        x_offset = 1;

    //if ((buf->ch_array)[curs_y][curs_x])
    change_colours(win->x + buf->curs_x + x_offset,
            win->y + buf->curs_y - win->text_offset, 1, 0x78, DIR_H);
    if (control_state == CS_COMMAND && view_global_message != 1)
        change_colours(command_line->curs_x, SCREENHEIGHT - 1, 1, 0x78, DIR_H);
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

// Sets the global message to the provided argument and sets it to be viewed
void set_global_message(wchar_t *message, unsigned char colour, ...) {
    // Resetting it from last time
    w_string_reset(global_message, MAX_BUFSIZE_SMALL);
    // Create a string from the extra arguments
    va_list args;  
    va_start(args, colour);
    //wchar_t *new_str = (wchar_t *)malloc(sizeof(wchar_t) * MAX_BUFSIZE_SMALL);
    vswprintf(global_message, message, args);
 
    /**
     * @NOTE : No check to see if the message is larger than MAX_BUFSIZE_SMALL 
     * But there is probably no need since this is called by me
     */
    //w_string_cpy(message, global_message);
    global_message_colour = colour;
    view_global_message = 1;
    return;
}

/**
 * Used to only initialise global lists, but now initialises file wide variables
 */
void init_lists() {
    b_len = 0;
    w_len = 0;
    buffers = calloc(MAX_BUFSIZE_TINY, sizeof(struct Buffer *));
    windows = calloc(MAX_BUFSIZE_TINY, sizeof(struct Window *));
    active_win = NULL;

    w_string_reset(global_message, MAX_BUFSIZE_SMALL); // No idea if this is required or not
    global_message_colour = 0x07;
    view_global_message = 0;
}

// Initialises the special command line buffer
struct Buffer *init_command_line() {
    struct Buffer *buf = malloc(sizeof(struct Buffer));

    wchar_t **tmp = calloc(MAX_BUFSIZE_MINI, sizeof(wchar_t *));
    for (int i = 0; i < MAX_BUFSIZE_TINY; i++)
        *(tmp + i) = calloc(MAX_BUFSIZE_MINI, sizeof(wchar_t));

    buf->ch_array   = tmp;
    buf->x_len_max  = MAX_BUFSIZE_MINI;
    buf->y_len_true = MAX_BUFSIZE_MINI;
    buf->y_len      = 1;

    buf->curs_x = 0;
    buf->curs_y = 0;

    return buf;
}

/**
 * Initialises a buffer
 * x_len_max and y_len are for presseting the size of ch_array, mainly for loading files
 * They can both be 0 and will be set to a preset size
 */
struct Buffer *init_buffer(int x_len_max, int y_len_true) {
    struct Buffer *buf = malloc(sizeof(struct Buffer));

    /**
     * Set up a basic array for the text
     * x_len_max is MAX_BUFSIZE_MINI (64)
     * y_len_true is MAX_BUFSIZE_TINY (16)
     */
    if (x_len_max > MAX_BUFSIZE_MINI)
        buf->x_len_max = x_len_max;
    else
        buf->x_len_max  = MAX_BUFSIZE_MINI;

    if (y_len_true) {
        buf->y_len      = y_len_true;
        buf->y_len_true = y_len_true;
    } else {
        buf->y_len      = 1;
        buf->y_len_true = MAX_BUFSIZE_TINY;
    }

    wchar_t **tmp = calloc(buf->y_len_true, sizeof(wchar_t *));
    for (int i = 0; i < buf->y_len_true; i++)
        *(tmp + i) = calloc(buf->x_len_max, sizeof(wchar_t));

    buf->ch_array   = tmp;

    w_string_reset(buf->fn_relative, MAX_BUFSIZE_LARGE);
    w_string_reset(buf->fn_absolute, MAX_BUFSIZE_LARGE);

    buf->curs_x = 0;
    buf->curs_y = 0;

    // Add the buffer to the list
    *(buffers + b_len) = buf;
    b_len++;

    return buf;
}

void deinit_buffer(struct Buffer *buf) {
    // @FIXME : No check to see if it's the last buffer
    // Freeing up the text
    for (int i = 0; i < buf->y_len_true; i++)
        free(*((buf->ch_array) + i));
    free(buf->ch_array);

    free(buf);
}

/**
 * Initialises a Window struct
 * If no buffer is provided then an empty one is created
 * DIR_V splits the current window in half vertically and DIR_H horizontally
 * @TODO : Actually finish this function
 */
struct Window *init_window(struct Buffer *buf, int x, int y, int width, int height) {
    struct Window *win = malloc(sizeof(struct Window));

    // If active_win is NULL then it's likely to be the first window that is created
    if (!active_win)
        active_win = win;

    win->x           = x;
    win->y           = y;
    win->width       = width;
    win->height      = height;
    win->buffer      = buf;
    win->text_offset = 0;

    // Add the window to the list
    *(windows + w_len) = win;
    w_len++;

    return win;
}

void deinit_window(struct Window *win) {
    free(win);
}

struct Window *get_active_window() {
    return active_win;
}

void set_active_window(struct Window *win) {
    active_win = win;
}

// Somewhat pointless since you can get the active buffer from the active window
struct Buffer *get_active_buffer() {
    return active_win->buffer;
}

const struct Window **get_window_list() {
    return (const struct Window **) windows;
}


/**
 * Finds what window is underneath the x and y value
 */
struct Window *find_window(int x, int y) {
    for (int i = 0; i < w_len; i++) {
        struct Window *win = *(windows + i);
        if (x >= win->x && x <= win->x + win->width &&
            y >= win->y && y <= win->y + win->height)
            return win;

    }

    // If we get here then the cursor is likely on the command line or something, which isn't a window
    return NULL;
}

/**
 * Finds a window adjacent to the window in the direction provided based on the cursor position
 * returns NULL if it couldn't find one
 */
struct Window *get_adjacent_win(const struct Window *win, int direction) {
    int x = win->buffer->curs_x + win->x;
    int y = win->buffer->curs_y + win->y;

    if (direction == DIR_LEFT) {
        while (x >= 0) {
            if (find_window(x, y) == active_win) x--;
            else return find_window(x, y);
        }
    } else if (direction == DIR_RIGHT) {
        while (x <= SCREENWIDTH) {
            if (find_window(x, y) == active_win) x++;
            else return find_window(x, y);
        }
    } else if (direction == DIR_UP) {
        while (y >= 0) {
            if (find_window(x, y) == active_win) y--;
            else return find_window(x, y);
        }
    } else if (direction == DIR_DOWN) {
        while (y <= SCREENHEIGHT) {
            if (find_window(x, y) == active_win) y++;
            else return find_window(x, y);
        }
    }

    return NULL;
}
