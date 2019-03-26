#ifndef UTILS_H
#define UTILS_H


/* Externs */
int power(int base, unsigned int expt);
void shift_pointers_right(void **ptr, size_t sz, int shift_len, int shift_pos);
void shift_pointers_left(void **ptr, size_t sz, int shift_len, int shift_pos);
int w_lines_in_file(const wchar_t *fn);
int w_widest_line_in_file(const wchar_t *fn);

/* String utility functions */
int w_string_cmp(const wchar_t *a, const wchar_t *b);
int w_string_cmp2(const wchar_t *a, const wchar_t *b, const size_t sz);
void w_string_cpy(const wchar_t *src, wchar_t *dest);
void w_string_cpy2(const wchar_t *src, wchar_t *dest, int len);
int w_string_len(const wchar_t *str);
void w_string_reset(wchar_t *str, size_t sz);
void w_string_cat(wchar_t *src, wchar_t *dest, size_t src_sz, size_t dest_sz);
void w_string_split(wchar_t *src, wchar_t *dest, size_t src_sz, size_t dest_sz, int split_pos);
void w_shift_chars_right(wchar_t *str, size_t sz, int shift_len, int shift_pos);
void w_shift_chars_left(wchar_t *str, size_t sz, int shift_len, int shift_pos);
int w_ch_in_str(const wchar_t ch, const wchar_t *str);
int w_str_to_int(const wchar_t *str);
unsigned char is_digit(wchar_t ch);
unsigned char is_alpha(wchar_t ch);
unsigned char is_special_ch(wchar_t ch);
wchar_t **line_wrap(const wchar_t *str, int len);
#endif
