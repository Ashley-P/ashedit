/**
 * This file handles any and all commands for manipulating text
 * including basic things such as entering text along with all the command line stuff
 */

#include <stdio.h>
#include "text.h"
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

void insert_char(struct Buffer *buf, int posx, int posy, wchar_t character) {
    w_shift_chars_right(*(buf->ch_array + posy), buf->x_len_max, 1, posx);
    *(*(buf->ch_array + posy) + posx) = character;
}

// Deletes the character at the position and not behind it
void delete_char(struct Buffer *buf, int posx, int posy) {
    w_shift_chars_left(*(buf->ch_array + posy), buf->x_len_max, 1, posx + 1);
}

// Places a line ahead of posy
void insert_line(struct Buffer *buf, int posy) {
    line_check(buf, 1);
    shift_pointers_right((void **)buf->ch_array, buf->y_len_true, 1, posy + 1);

    // Allocating space for the line created
    *(buf->ch_array + posy + 1) = calloc(buf->x_len_max, sizeof(wchar_t));

    buf->y_len++;
}

void delete_line(struct Buffer *buf, int posy) {
    shift_pointers_left((void **) buf->ch_array, buf->y_len_true, 1, posy);

    // Allocating space for the line created
    *(buf->ch_array + buf->y_len_true - 1) = calloc(buf->x_len_max, sizeof(wchar_t));

    buf->y_len--;
}

/**
 * Saves memory at the pointer to a file
 * This is a lower level function that should be called by other functions
 */
int save(const wchar_t **text, int y_len, const wchar_t *fn) {
    FILE *f = _wfopen(fn, L"w");

    if (!f) {
        set_global_message(L"Could not open %ls for writing", 0x04, fn);
        return 0;
    }
    // Start writing to file
    // fwprintf is used so we can append a newline to each string
    for (int i = 0; i < y_len; i++)
        //fputws(*(text + i), f);
        fwprintf(f, L"%ls\n", *(text + i));

    set_global_message(L"File \"%ls\" successfully saved", 0x07, fn);
    fclose(f);
    return 1;
}

int load(wchar_t **text, int x_len, const wchar_t *fn) {
    FILE *f = _wfopen(fn, L"r");

    if (!f) {
        set_global_message(L"Could not open %ls for reading", 0x04, fn);
        return 0;
    }

    // Start reading from the file
    int i = 0;
    while (fgetws(*(text + i), x_len, f) != NULL) {
        // strip the newlines from the string
        w_shift_chars_left(*(text + i), x_len, 1, w_string_len(*(text + i)) + 1);
        i++;
    }

    set_global_message(L"File \"%ls\" succesfully loaded", 0x07, fn);
    fclose(f);
    return 1;
}
