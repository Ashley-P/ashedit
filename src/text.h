#ifndef TEXT_H
#define TEXT_H

void redraw_screen();
void init_lists();
void set_global_message(wchar_t *message, unsigned char colour);
struct Buffer *init_command_line();
struct Buffer *init_buffer();
void deinit_buffer(struct Buffer *buf);
struct Window *init_window();
void deinit_window(struct Window *win);
struct Window *get_active_window();
void set_active_window(struct Window *win);
struct Buffer *get_active_buffer();

#endif
