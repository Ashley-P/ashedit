#ifndef DEFS_H
#define DEFS_H

#include "windows.h"

// For initialisation
#define smallSMALLRECT ((SMALL_RECT) {0, 0, 1, 1})
#define CURSORINFO     ((CONSOLE_CURSOR_INFO) {1, 0})
#define COORDsize      ((COORD) {(short) SCREENWIDTH, (short) SCREENHEIGHT})
#define SMALLRECTsize  ((SMALL_RECT) {0, 0, (short) SCREENWIDTH - 1, (short) SCREENHEIGHT - 1})

#define SCREENWIDTH  100
#define SCREENHEIGHT 30

#define DIR_H 1
#define DIR_V 2

#define MAX_BUFSIZE_TINY  16
#define MAX_BUFSIZE_MINI  64
#define MAX_BUFSIZE_SMALL 256
#define MAX_BUFSIZE_MED   1024 
#define MAX_BUFSIZE_LARGE 2048
#define MAX_BUFSIZE_SUPER 100000

enum ControlState {
    CS_NULL      = 0,
    CS_EDIT      = 0x1,
    CS_COMMAND   = 0x2,
};

// Extern Variables
extern HANDLE    stdin;
extern HANDLE    console;
extern CHAR_INFO *screen;

int isRunning; // Whether the program is running or not

#endif
