#include "defs.h"



void insert_char(struct Buffer *buf, int posx, int posy, wchar_t character);
void delete_char(struct Buffer *buf, int posx, int posy);
void insert_line(struct Buffer *buf, int posy);
void delete_line(struct Buffer *buf, int posy);
int save(const wchar_t **text, int len, const wchar_t *fn);
