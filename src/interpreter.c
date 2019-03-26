/**
 * This file handles the intepreter for the command line
 */
#include "commands.h"
#include "defs.h"
#include "text.h"
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

static inline wchar_t scanner_getch(const wchar_t *line) {
    return *(line + scanner_pos++);
}

static inline wchar_t scanner_peek(const wchar_t *line) {
    return *(line + scanner_pos + 1);
}

// Defines to make the lexer look a bit cleaner
#define T_VALUE (tokens + token_num)->value
#define T_TYPE  (tokens + token_num)->type

struct Token *lexer(const wchar_t *line) {
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
                ch = scanner_getch(line);

            // Integers
            } else if (is_digit(ch)) {
                T_TYPE = TT_ARG_INT;
                while(is_digit(ch)) {
                    T_VALUE[a++] = ch;
                    ch = scanner_getch(line);
                }
                // Peek ahead to see if the string is correct
                if (ch != L'\0' && ch != L' ') {
                    // Error Message
                    goto error;
                }
            // Normal args and keywords
            } else {
                T_TYPE = TT_ARG_STD;
                //ch = scanner_getch(line);
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

/**
 * Checks the arguments of a token list based on the provided enum list
 * Note that TT_COMMAND shouldn't be in the types
 * TT_EOL has to be in the types or else bad stuff will happen because it's required
 * to know the end of the list (like '\0')
 */
int argument_checker(const struct Token *tokens, const enum TokenType *types) {
    for (int i = 0;; i++) {
        if ((tokens + i + 1)->type == TT_EOL && *(types + i) == TT_EOL) return 1;
        else if ((tokens + i + 1)->type == TT_EOL || *(types + i) == TT_EOL) return 0;
        else if ((tokens + i + 1)->type != *(types + i)) return 0;
        else continue;
    }
}

void parser(const struct Token *tokens) {
    // Big if else statement for all the commands
    if (w_string_cmp(tokens->value, L"save")) {
        /**
         * Right now we just save the active buffer to a file
         * If there is a second argument then we try saving to that
         * If there is no argument then we try saving to fn_absolute/fn_relative
         * If those don't exist then we put up an error
         */
        enum TokenType arr1[] = {TT_ARG_STR, TT_EOL};
        enum TokenType arr2[] = {TT_KEYWORD, TT_EOL};
        enum TokenType arr3[] = {TT_EOL};
        const struct Buffer *buf = get_active_buffer();
        if (argument_checker(tokens, arr1)) {
            // @TODO: If the file saves sucessfuly copy the new filename into fn_*
            save((const wchar_t **) buf->ch_array, buf->y_len, (tokens + 1)->value);

        } else if (argument_checker(tokens, arr2)) { // For the keyword "ALL"
            if (w_string_cmp((tokens + 1)->value, L"ALL")) {
                // Here we would cycle through all the buffers and try saving them
                set_global_message(L"Keyword!", 0x07);
            } else
                set_global_message(L"Wrong Keyword!", 0x07);

        } else if (argument_checker(tokens, arr3)) {
            // Check if fn_absolute or fn_relative exists
            if (*buf->fn_absolute != L'\0')
                save((const wchar_t **) buf->ch_array, buf->y_len, buf->fn_absolute);
            if (*buf->fn_relative != L'\0')
                save((const wchar_t **) buf->ch_array, buf->y_len, buf->fn_relative);
            else // If we get here then there is no file name in the argument or saved in the buffer
                set_global_message(L"No filename provided for buffer", 0x07);

        } else
            // @TODO: Should probably expand this error a bit more
            set_global_message(L"Incorrect number and/or type of arguments", 0x04);


    } else if (w_string_cmp(tokens->value, L"load")) {
        /**
         * We need to create a buffer to load a file and a quick check on the arguments
         * @TODO: Modify to able to accept a ton of arguments and load them all
         */ 
        enum TokenType arr1[] = {TT_ARG_STR, TT_EOL};
        if (argument_checker(tokens, arr1)) {
            // Find the y_len and x_len_max of the file
            // @FIXME: Buffer kind of just stays around even if the file doesn't exist
            int y_cnt = w_lines_in_file((tokens + 1)->value);
            int x_cnt  = w_widest_line_in_file((tokens + 1)->value);
            struct Buffer *buf = init_buffer(x_cnt, y_cnt);
            load(buf->ch_array, buf->x_len_max, (tokens + 1)->value);
            // Set the buffer in the active window
            struct Window *active_win = get_active_window();
            active_win->buffer = buf;

        } else
            // @TODO: Should probably expand this error a bit more
            set_global_message(L"Incorrect number and/or type of arguments", 0x04);

    } else if (w_string_cmp(tokens->value, L"split")) {
        set_global_message(L"NOT IMPLEMENTED", 0x0B);

    } else if (w_string_cmp(tokens->value, L"vsplit")) {
        set_global_message(L"NOT IMPLEMENTED", 0x0B);

    } else if (w_string_cmp(tokens->value, L"man")) { // manual/help
        set_global_message(L"NOT IMPLEMENTED", 0x0B);
        
    } else { // Otherwise it's an unknown command and we should send a message
        set_global_message(L"Unknown command \"%s\"", 0x04, tokens->value);
    } 
}

void interpreter(wchar_t *line) {
    struct Token *tokens = lexer(line);

    // Check here to see if the lexer return with an error
    if (tokens->type == TT_NULL)
        // @FIXME: Make a proper display of the error
        set_global_message(L"Lexer error!", 0x04);
    else
        parser(tokens);

    deinit_tokens(tokens);
    scanner_pos = 0;
}
