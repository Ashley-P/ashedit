#include <stdarg.h>
#include <stdlib.h>
#include <wchar.h>
#include "defs.h"
#include "utils.h"


int power(int base, unsigned int expt) {
    int result = 1;
    for (int i = 0; i < expt; i++) {
        result *= base;
    }
    return base;
}

// Shift pointers down an array
void shift_pointers_right(void **ptr, size_t sz, int shift_len, int shift_pos) {
    // Free up the pointers that will be overwritten
    for (int i = 0; i < shift_len; i++)
        free(*(ptr + sz - 1 - i));

    for (int i = sz; i >= shift_pos; i--) {
        if (i + shift_len >= sz) continue;
        else *(ptr + i + shift_len) = *(ptr + i);
    }

    // Making sure the space created has NULL pointers
    for (int i = 0; i < shift_len; i++)
        *(ptr + i + shift_pos) = NULL;
}

void shift_pointers_left(void **ptr, size_t sz, int shift_len, int shift_pos) {
    // Free up the pointers that will be overwritten
    for (int i = 0; i < shift_len; i++)
        free(*(ptr + shift_pos - i));

    for (int i = shift_pos; i < sz; i++) {
        if (i - shift_len < 0) continue;
        else *(ptr + i - shift_len) = *(ptr + i);
    }

    // Making sure the pointers at the end are NULL
    for (int i = 0; i < shift_len; i++)
        *(ptr + sz - 1 - i) = NULL;
}

int w_lines_in_file(const wchar_t *fn) {
    FILE *f = _wfopen(fn, L"r");
    if (!f) return -1;

    int cnt = 0;
    wchar_t ch;
    while ((ch = fgetwc(f)) != WEOF) {
        if (ch == L'\n') cnt++;
    }

    return cnt;
}

int w_widest_line_in_file(const wchar_t *fn) {
    FILE *f = _wfopen(fn, L"r");
    if (!f) return -1;

    int cnt = 0;
    int max = 0;
    wchar_t ch;
    while ((ch = fgetwc(f)) != WEOF) {
        if (ch != L'\n' && ch != L'\0')
            cnt++;
        else {
            max = max > cnt ? max : cnt;
            cnt = 0;
        }
    }

    return max;
}

/********* String utility functions *********/
/**
 * wchar_t string comparing
 * Requires null-terminated strings
 */


int w_string_cmp(const wchar_t *a, const wchar_t *b) {
    for (int i = 0;; i++) {
        if (*(a + i) == L'\0' && *(b + i) == L'\0') {
            return 1;
        } else if (*(a + i) == *(b + i)) {
            continue;
        } else 
            return 0;
    }
}

/* Same as above except it compares to a size */
int w_string_cmp2(const wchar_t *a, const wchar_t *b, const size_t sz) {
    for (int i = 0; i < sz ; i++) {
        if (*(a + i) == L'\0' && *(b + i) == L'\0') {
            return 1;
        } else if (*(a + i) == *(b + i)) {
            continue;
        } else 
            return 0;
    }

    return 1;
}

/**
 * wchar_t string copying
 * Requires null-terminated strings
 * Undefined behaviour if src is larger than dest
 */
void w_string_cpy(const wchar_t *src, wchar_t *dest) {
    for (int i = 0; *(src + i) != L'\0'; i++)
        *(dest + i) = *(src + i);
}

/**
 *  Same as w_string_cpy except it only copies upto len
 */
void w_string_cpy2(const wchar_t *src, wchar_t *dest, int len) {
    for (int i = 0; *(src + i) != L'\0' && i < len; i++)
        *(dest + i) = *(src + i);
}

/**
 * wchar_t string length 
 * Requires null-terminated strings
 */
int w_string_len(const wchar_t *str) {
    int i = 0;
    while (*(str + i) != L'\0' && *(str + i) != L'\n') {
        i++;
    }

    return i;
}

/**
 * This functions moves src onto the end of dest
 * The end of dest is considered to be when the first '\0' character is found
 * src isn't freed
 */
void w_string_cat(wchar_t *src, wchar_t *dest, size_t src_sz, size_t dest_sz) {
    // Calculate size of string using w_string_len
    int dest_len = w_string_len(dest);

    for (int i = 0; i < src_sz || i + dest_len < dest_sz; i++) {
        *(dest + i + dest_len) = *(src + i);
        //*(src + i) = L'\0';
    }
}

void w_string_split(wchar_t *src, wchar_t *dest, size_t src_sz, size_t dest_sz, int split_pos) {
    for (int i = 0; i + split_pos < src_sz || i < dest_sz; i++) {
        *(dest + i) = *(src + split_pos + i);
        *(src + split_pos + i) = L'\0';
    }
}

void w_string_reset(wchar_t *str, size_t sz) {
    for (int i = 0; i < sz; i++)
        *(str + i) = L'\0';
}

void w_shift_chars_right(wchar_t *str, size_t sz, int shift_len, int shift_pos) {
    for (int i = sz; i >= shift_pos; i--) {
        if (i + shift_len >= sz) continue;
        else *(str + i + shift_len) = *(str + i);
    }
    *(str + shift_pos) = L'\0';

    // Null Terminate the string
    *(str + sz - 1) = L'\0';
}

void w_shift_chars_left(wchar_t *str, size_t sz, int shift_len, int shift_pos) {
    for (int i = shift_pos; i <= sz; i++) {
        //if (i + shift_len >= sz) continue;
        if (i - shift_len < 0) continue;
        else *(str + i - shift_len) = *(str + i);
    }

    for (int i = 0; i < shift_len; i++) {
        *(str + sz - 1 - i) = L'\0';
    }

}

int w_ch_in_str(const wchar_t ch, const wchar_t *str) {
    for (int i = 0;; i++) {
        if (*(str + i) == L'\0') return 0;
        else if (ch == *(str + i)) return 1;
        else continue;
    }

}

int w_str_to_int(const wchar_t *str) {
    int len = w_string_len(str);
    int a = 0;
    for (int i = 0; i < len; i++) {
        a += ((int) *(str + i)) - 48;
        a *= 10;
    }
    a /= 10;
    return a;
}

unsigned char is_digit(wchar_t ch) {
    if (ch >= L'0' && ch <= L'9') return 1;
    else return 0;
}

unsigned char is_alpha(wchar_t ch) {
    if (ch >= L'a' && ch <= L'z') return 1;
    else if (ch >= L'A' && ch <= L'Z') return 1;
    else return 0;
}

unsigned char is_special_ch(wchar_t ch) {
    // @FIXME : Fix the special char array
    wchar_t special[] = L".,_-\"\'\\/();:";
    for (int i = 0; i < w_string_len(special); i++) {
        if (ch == special[i])
            return 1;
    }
    return 0;
}

/**
 * Takes an argument to a string and the length that you want the lines to be, and wraps it preserving
 * words. The array returned is of MAX_BUFSIZE_MINI, but will contain an empty array at the end of the
 * similar to '\0' at the end of a string
 * @NOTE : Doesn't free the provided string
 * @NOTE @FIXME : Will probably break if there is a word in the string that is longer than the len
 */
wchar_t **line_wrap(const wchar_t *str, int len) {
    if (len < 2) {
        //d_debug_message(0x0C, ERROR_D, L"Error in line_wrap: len is less than 2. (%d)", len);
        return NULL;
    }
    // Allocating the size, each array is of size len + 1
    wchar_t **rtn_str = calloc(MAX_BUFSIZE_MINI, sizeof(wchar_t *));
    for (int i = 0; i < MAX_BUFSIZE_MINI; i++) {
        *(rtn_str + i) = calloc(len + 1, sizeof(wchar_t));
    }

    int str_cnt  = 0;
    int ch_cnt   = 0; // what characther we are on
    int line_cnt = 0; // What line we are on
    /**
     * Count of characters in a word. A word being some alphanum characters delimited by spaces, other
     * characters are counted in this to make the wrapping look proper (e.g It won't split a full stop
     * into it's own line
     */
    int word_ch_cnt = 0;
    wchar_t *line = rtn_str[0];
    int j = 0;

    while (*(str + str_cnt) != '\0') {
        // If we are at the end of a line or we hit a newline we start a new line
        if (ch_cnt == len || *(str + str_cnt) == L'\n') {
            if (*(str + str_cnt) == L'\n') {
                *(line + ch_cnt) = *(str + str_cnt);
                str_cnt++;
            }
            ch_cnt = 0;
            line_cnt++;
            line = &rtn_str[line_cnt][0];
            continue;
        }

        // If we are at the start of a line and there is a space we just skip it.
        if (ch_cnt == 0 && *(str + str_cnt) == L' ') {
            str_cnt++;
            continue;
        }
            
        // Start counting the number of characters in the next word
        if (is_alpha(*(str + str_cnt)) || is_digit(*(str + str_cnt)) || is_special_ch(*(str + str_cnt))) {
            word_ch_cnt = 0;
            j = str_cnt;
            while (*(str + j) != L'\0' && *(str + j) != L'\n' && *(str + j) != L' ') {
                word_ch_cnt++;
                j++;
            }

            // @FIXME @NOTE : Implement this at some point
            // If the word is bigger than len then we print an error and return the half built string
            if (word_ch_cnt > len) {
                /*
                // Copy the word into a temporary array so we can put it in the error message
                wchar_t temp[word_ch_cnt];
                for (int i = 0; i < word_ch_cnt; i++)
                    temp[i] = *(str + str_cnt + i);

                d_debug_message(0x0C, ERROR_D, L"Error in line_wrap: word %ls is too long (%d/%d)",
                        temp, word_ch_cnt, len);
                 */
                return rtn_str;
            // If the word goes over the length, then we just go to the next line
            } else if (word_ch_cnt + ch_cnt > len) {
                ch_cnt = 0;
                line_cnt++;
                line = &rtn_str[line_cnt][0];
            }
            // Then we add the word to the line
            for (int k = 0; k < word_ch_cnt; k++)
                *(line + ch_cnt + k) = *(str + str_cnt + k);

            ch_cnt += word_ch_cnt;
            str_cnt += word_ch_cnt;

            continue;
        }

        *(line + ch_cnt) = *(str + str_cnt);
        str_cnt++;
        ch_cnt++;
    }

    return rtn_str;
}
