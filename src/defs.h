#ifndef DEFS_H
#define DEFS_H

#include <stdio.h>
#include <windows.h>

// For initialisation
#define smallSMALLRECT ((SMALL_RECT) {0, 0, 1, 1})
#define CURSORINFO     ((CONSOLE_CURSOR_INFO) {1, 0})
#define COORDsize      ((COORD) {(short) SCREENWIDTH, (short) SCREENHEIGHT})
#define SMALLRECTsize  ((SMALL_RECT) {0, 0, (short) SCREENWIDTH - 1, (short) SCREENHEIGHT - 1})

#define SCREENWIDTH  100
#define SCREENHEIGHT 30

#define DIR_H 1
#define DIR_V 2

#define DIR_LEFT   1
#define DIR_RIGHT  2
#define DIR_UP     3
#define DIR_DOWN   4


#define MAX_BUFSIZE_TINY  16
#define MAX_BUFSIZE_MINI  64
#define MAX_BUFSIZE_SMALL 256
#define MAX_BUFSIZE_MED   1024 
#define MAX_BUFSIZE_LARGE 2048
#define MAX_BUFSIZE_SUPER 100000

/* For Drawing the borders */
#define DOUBLE_HORIZONTAL     L'\u2550'
#define DOUBLE_VERTICAL       L'\u2551'
#define DOUBLE_DOWN_AND_RIGHT L'\u2554'
#define DOUBLE_DOWN_AND_LEFT  L'\u2557'
#define DOUBLE_UP_AND_RIGHT   L'\u255A'
#define DOUBLE_UP_AND_LEFT    L'\u255D'

#define LIGHT_HORIZONTAL      L'\u2500'
#define LIGHT_VERTICAL        L'\u2502'
#define LIGHT_DOWN_AND_RIGHT  L'\u250C'
#define LIGHT_DOWN_AND_LEFT   L'\u2510'
#define LIGHT_UP_AND_RIGHT    L'\u2514'
#define LIGHT_UP_AND_LEFT     L'\u2518'

// Enums
enum ControlState {
    CS_NULL      = 0,
    CS_EDIT      = 0x1,
    CS_COMMAND   = 0x2,
    
    CS_ANY       = CS_EDIT | CS_COMMAND,
};

// Structs

/**
 * Struct for handling the buffer
 * Has a pointer to the 2d array that the actual text will be on
 * Has both filenames because I'm not sure if I need only one of them
 * Also contains where the cursor should be if it is the active buffer
 */
struct Buffer {
    // Pointer to the character array that will be typed to
    wchar_t **ch_array;

    
    // Max length of a string in the buffer
    int x_len_max;

    // How many lines the array has (y_len_true is how many lines have been allocated in memory)
    int y_len;
    int y_len_true;

    // Filenames
    wchar_t fn_relative[MAX_BUFSIZE_LARGE];
    wchar_t fn_absolute[MAX_BUFSIZE_LARGE];

    // Cursor stuff @NOTE @TODO: Should probably attach this to the window rather than the buffer
    int curs_x;
    int curs_y;

    // Special variable for the command line buffer, left uninitialised in normal buffers
    int command_curs_y;

};

/**
 * This is for helping to display buffers since you can/should be able to have
 * more buffers than you can display on screen
 */
struct Window {
    // Position and size stuff
    int x;
    int y;
    int width;
    int height;

    // The buffer that should be displayed
    struct Buffer *buffer;

    // the offset at which we should be viewing the text
    int text_offset;
};

// Extern Variables
extern HANDLE    h_stdin;
extern HANDLE    console;
extern CHAR_INFO *screen;
extern int isRunning; // Whether the program is running or not
extern struct Buffer *command_line;
extern enum ControlState control_state;

extern unsigned char global_message_colour;
extern int view_global_message;

#endif
