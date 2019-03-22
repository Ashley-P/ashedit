/**
 * This file handles the intepreter for the command line
 */
#include "defs.h"
#include "interpreter.h"
#include "utils.h"

// File globals
int scanner_pos;
enum LexerState lexer_state;
static int keyword_len = 1;
static wchar_t *keywords[] = {L"ALL",};

static inline struct Token *init_tokens() {
    struct Token *ptr = malloc(MAX_BUFSIZE_TINY * sizeof(struct Token));

    for (int i = 0; i < MAX_BUFSIZE_TINY; i++) {
        (ptr + i)->value    = calloc(MAX_BUFSIZE_SMALL, sizeof(wchar_t));
        (ptr + i)->type     = TT_NULL;
    }

    return ptr;
}

static inline void deinit_tokens(struct Token *tokens) {
    for (int i = 0; i < MAX_BUFSIZE_TINY; i++)
        free(tokens + i);
    free(tokens);
}

static inline wchar_t scanner_getch(wchar_t *line) {
    return *(line + scanner_pos++);
}

static inline wchar_t scanner_peek(wchar_t *line) {
    return *(line + scanner_pos + 1);
}

// Defines to make the lexer look a bit cleaner
#define T_VALUE (tokens + token_num)->value
#define T_TYPE  (tokens + token_num)->type

struct Token *lexer(wchar_t *line) {
    lexer_state = LS_START;
    struct Token *tokens = init_tokens();
    int a = 0;
    int token_num = 0;
    wchar_t ch = scanner_getch(line);
    while (1) {
        // On the highest level we expect that we are at the start of a token

        if (ch == L' ') {
            token_num++;
            a = 0;
            ch = scanner_getch(line);

        } else if (ch == L'\0') {
            token_num++;
            T_VALUE[a++] = L'\0';
            T_TYPE       = TT_EOL;
            break;

        } else if (lexer_state == LS_START) {
            // If we are at the start of the string then we expect to find a command
            // commands start with an alpha character
            if (is_alpha(ch)) {
                T_TYPE = TT_COMMAND;
                T_VALUE[a++] = ch;

                while(is_alpha(ch = scanner_getch(line)))
                    T_VALUE[a++] = ch;

                lexer_state = LS_MID;
            } else {
                // Error message would go here
                goto error;
            }

        // Arguments and keywords go here
        } else if (lexer_state == LS_MID) {
            // Strings
            if (ch == L'"') {
                T_TYPE = TT_ARG_STR;
                // Just find the other double quote
                while (1) {
                    ch = scanner_getch(line);
                    if (ch == L'"') break;
                    else if (ch == L'\0') {
                        // Error message
                        goto error;
                    } else T_VALUE[a++] = ch;
                }

            // Integers
            } else if (is_digit(ch)) {
                T_TYPE = TT_ARG_INT;
                while(is_digit(ch = scanner_getch(line))) {
                    T_VALUE[a++] = ch;
                }
                // Peek ahead to see if the string is correct
                wchar_t peek = scanner_peek(line);
                if (peek != L'\0' && peek != L' ') {
                    // Error Message
                    goto error;
                }
            // Normal args and keywords
            } else {
                T_TYPE = TT_ARG_STD;
                ch = scanner_getch(line);
                while (ch != L'\0' && ch != L' ') {
                    T_VALUE[a++] = ch;
                    ch = scanner_getch(line);
                }

                // Check if the argument is a keyword
                for (int i = 0; i < keyword_len; i++) {
                    if (w_string_cmp(T_VALUE, keywords[i]))
                        T_TYPE = TT_KEYWORD;
                }
            }
        }
    }

    return tokens;

error:
    w_string_cpy(L"INVALID", tokens->value);
    tokens->type = TT_NULL;
    return tokens;
}

void parser(struct Token *tokens) {}

void interpreter(wchar_t *line) {
    struct Token *tokens = lexer(line);
    // Check here to see if the lexer return with an error
    parser(tokens);


cleanup:
    deinit_tokens(tokens);
    scanner_pos = 0;
}
