/**
 * This file handles any and all commands for manipulating text
 * including basic things such as entering text
 */

#include "commands.h"
#include "utils.h"


void insert_char(struct Buffer *buf, wchar_t character) {
    w_shift_chars_right(*(buf->ch_array + buf->curs_y), buf->x_len_max, 1, buf->curs_x);
    *(*(buf->ch_array + buf->curs_y) + buf->curs_x) = character;
    buf->curs_x++;
}
