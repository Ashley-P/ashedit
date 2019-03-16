/**
 * This file handles anything text specific that isn't already handling in utils.c
 * namely initializing and deinitalising buffers and windows
 * along with handling their usage
 */

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

/**
 * @NOTE : Implement Line numbering
 * @NOTE : Implement Line wrapping
 */
void draw_ui() {
    // Bottom row is for typing commands so it's left blank

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
            draw_char_line(win->x, win->y, LIGHT_HORIZONTAL, win->height, 0x70, DIR_H);
            x_offset = 1;
        }

        // Drawing each line of text
        // -1 because the bottom bar is part of the window
        int j;
        for (j = 0; j < win->height - 1; j++) {
            // Start from the offset
            if ((j + win->text_offset) >= buf->y_len) break;
            draw_chars(win->x + x_offset, win->y + j, *(buf->ch_array + win->text_offset + j), + 0x07);
        }

        // Draw Tildes on the rest of the window
        for (; j < win->height; j++)
            draw_char(win->x + x_offset, win->y + j, L'~', 0x07);


        // Drawing the bottom border of the window
        change_colours(win->x, win->y + win->height, win->width, 0x70, DIR_H);
        if (w_string_cmp(buf->fn_relative, L"\0"))
            draw_chars(win->x, win->y + win->height, L"New File", 0x70);
        else
            draw_chars(win->x, win->y + win->height, buf->fn_relative, 0x70);
    }


    // Draw the cursor wherever it's supposed to be
    struct Window *win = active_win;
    struct Buffer *buf = win->buffer;

    if (win->x == 0)
        x_offset = 0;
    else
        x_offset = 1;

    //if ((buf->ch_array)[curs_y][curs_x])
    change_colours(win->x + buf->curs_x + x_offset, win->y + buf->curs_y, 1, 0x78, DIR_H);
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


// Initialises the global lists
void init_lists() {
    b_len = 0;
    w_len = 0;
    buffers = calloc(MAX_BUFSIZE_TINY, sizeof(struct Buffer *));
    windows = calloc(MAX_BUFSIZE_TINY, sizeof(struct Window *));
    active_win = NULL;
}

/**
 * Initialises a buffer
 * Most of the values are set programatically so nothing is required in the arguments
 * @NOTE : init doesn't open a file automatically but deinit closes on if there is one
 * @NOTE : Implement construction if a file handle is provided
 */
struct Buffer *init_buffer(FILE *handle) {
    struct Buffer *buf = malloc(sizeof(struct Buffer));

    /**
     * Set up a basic array for the text
     * x_len_max is MAX_BUFSIZE_MINI (64)
     * y_len_true is MAX_BUFSIZE_TINY (16)
     */
    wchar_t **tmp = calloc(MAX_BUFSIZE_TINY, sizeof(wchar_t *));
    for (int i = 0; i < MAX_BUFSIZE_TINY; i++)
        *(tmp + i) = calloc(MAX_BUFSIZE_MINI, sizeof(wchar_t));

    buf->ch_array   = tmp;
    buf->x_len_max  = MAX_BUFSIZE_MINI;
    buf->y_len_true = MAX_BUFSIZE_TINY;
    buf->y_len      = 1;

    //w_string_cpy(L"\0", buf->fn_relative);
    //w_string_cpy(L"\0", buf->fn_absolute);
    for (int i = 0; i < MAX_BUFSIZE_LARGE; i++) {
        *(buf->fn_relative + i) = L'\0';
        *(buf->fn_absolute + i) = L'\0';
    }
    buf->handle = NULL;

    buf->curs_x = 0;
    buf->curs_y = 0;

    // Add the buffer to the list
    *(buffers + b_len) = buf;
    b_len++;

    return buf;
}

void deinit_buffer(struct Buffer *buf) {
    // Freeing up the text
    for (int i = 0; i < buf->y_len_true; i++)
        free(*((buf->ch_array) + i));
    free(buf->ch_array);

    // Closing the file if it hasn't been closed already
    if (buf->handle)
        fclose(buf->handle);

    free(buf);
}

/**
 * Initialises a Window struct
 * If no buffer is provided then an empty one is created
 * DIR_V splits the current window in half vertically and DIR_H horizontally
 * @NOTE : Actually finish this function
 */
struct Window *init_window(struct Buffer *buf, int direction) {
    struct Window *win = malloc(sizeof(struct Window));
    // Get the active window and split it in half

    // If active_win is NULL then it's likely to be the first window that is created
    if (!active_win) {
        active_win = win;
        win->x           = 0;
        win->y           = 0;
        win->width       = SCREENWIDTH;
        win->height      = SCREENHEIGHT - 2;
        win->buffer      = buf;
        win->text_offset = 0;
    }

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

// Someone pointless since you can get the active buffer from the active window
struct Buffer *get_active_buffer() {
    return active_win->buffer;
}
