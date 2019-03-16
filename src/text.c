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

void draw_ui() {
    // Bottom row is for typing commands so it's left blank

#if 1
    // Some test that will get removed at some point
    draw_char_line(0, 0, L'A', 10, 0x89, DIR_H);
#endif
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
    b_len = MAX_BUFSIZE_TINY;
    w_len = MAX_BUFSIZE_TINY;
    buffers = malloc(sizeof(struct Buffer *) * b_len);
    windows = malloc(sizeof(struct Window *) * w_len);
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
    wchar_t **tmp = malloc(sizeof(wchar_t *) * MAX_BUFSIZE_TINY);
    for (int i = 0; i < MAX_BUFSIZE_TINY; i++)
        *(tmp + i) = malloc(sizeof(wchar_t) * MAX_BUFSIZE_MINI);

    buf->ch_array   = tmp;
    buf->x_len_max  = MAX_BUFSIZE_MINI;
    buf->y_len_true = MAX_BUFSIZE_TINY;
    buf->y_len      = 1;

    w_string_cpy(L"\0", buf->fn_relative);
    w_string_cpy(L"\0", buf->fn_absolute);
    buf->handle = NULL;

    buf->curs_x = 0;
    buf->curs_y = 0;

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
 */
struct Window *init_window(struct Buffer *buf, int direction) {
    struct Window *win = malloc(sizeof(struct Window));
    // Get the active window and split it in half

    // If active_win is NULL then it's likely to be the first window that is created
    if (!active_win) {
        active_win = win;
        win->x = 0;
        win->y = 0;
        win->width = SCREENWIDTH;
        win->height = SCREENHEIGHT - 2;
    }

    return win;
}

void deinit_window(struct Window *win);

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
