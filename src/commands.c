/**
 * This file handles any and all commands for manipulating text
 * including basic things such as entering text
 */

#include "commands.h"
#include "utils.h"


/**
 * This function checks if there is enough lines allocated in memory
 */
void line_check(struct Buffer *buf, int lines_wanted) {
    if (lines_wanted >= buf->y_len_true - buf->y_len) {
        //buf->ch_array = realloc(buf->ch_array, buf->y_len_true + MAX_BUFSIZE_TINY);
        // Doing it manually
        wchar_t **old = buf->ch_array;
        buf->ch_array = malloc((buf->y_len_true + MAX_BUFSIZE_TINY) * sizeof(wchar_t *));
        //Copy the pointers into the new array
        for (int i = 0; i < buf->y_len_true; i++)
            *(buf->ch_array + i) = *(old + i);

        free(old);

        // Allocate more memory for the extra space in the array
        for (int i = 0; i < MAX_BUFSIZE_TINY; i++)
            *(buf->ch_array + buf->y_len_true + i) = calloc(buf->x_len_max, sizeof(wchar_t));

        buf->y_len_true += MAX_BUFSIZE_TINY;
    }
}

void insert_char(struct Buffer *buf, wchar_t character) {
    w_shift_chars_right(*(buf->ch_array + buf->curs_y), buf->x_len_max, 1, buf->curs_x);
    *(*(buf->ch_array + buf->curs_y) + buf->curs_x) = character;
    buf->curs_x++;
}

void delete_char(struct Buffer *buf) {
    w_shift_chars_left(*(buf->ch_array + buf->curs_y), buf->x_len_max, 1, buf->curs_x);
    buf->curs_x--;
}

// Puts whatever to the right of the cursor on the next line
void new_line(struct Buffer *buf) {
    line_check(buf, 1);
    shift_pointers_right(buf->ch_array, buf->y_len_true, 1, buf->curs_y + 1);
    *(buf->ch_array + buf->curs_y + 1) = calloc(buf->x_len_max, sizeof(wchar_t));
    buf->y_len++;
    buf->curs_y++;
    buf->curs_x = 0;
}
