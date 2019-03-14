#ifndef DEFS_H
#define DEFS_H

// For initialisation
#define smallSMALLRECT ((SMALL_RECT) {0, 0, 1, 1})
#define CURSORINFO     ((CONSOLE_CURSOR_INFO) {1, 0})
#define COORDsize      ((COORD) {(short) SCREENWIDTH, (short) SCREENHEIGHT})
#define SMALLRECTsize  ((SMALL_RECT) {0, 0, (short) SCREENWIDTH - 1, (short) SCREENHEIGHT - 1})

#define SCREENWIDTH  50
#define SCREENHEIGHT 30

enum ControlState {
    CS_NULL      = 0,
    CS_EDIT      = 0x1,
    CS_COMMAND   = 0x2,
};

// Extern Variables
extern HANDLE    stdin;
extern HANDLE    console;
extern CHAR_INFO *screen;

#endif
