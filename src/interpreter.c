/**
 * This file handles the intepreter for the command line
 */
#include "defs.h"
#include "interpreter.h"

enum LexerState lexer_state;

struct Token *lexer(wchar_t *line) {
    lexer_state = LS_START;
    struct Token *tokens = malloc(MAX_BUFSIZE_TINY * sizeof(struct Token));

    return tokens;
}

void parser() {}

void interpreter(wchar_t *line) {
    struct Token *tokens = lexer(line);
}
