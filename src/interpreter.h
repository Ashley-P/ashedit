enum TokenType {
    TT_NULL = 0,
    TT_COMMAND = 0x1,

    TT_ARG_STD = 0x2,
    TT_ARG_INT = 0x4,
    TT_ARG_STR = 0x8,

    TT_ARG_ANY = TT_ARG_STD | TT_ARG_INT | TT_ARG_STR,

    TT_KEYWORD = 0x16,
    TT_EOL     = 0x32,
};

struct Token {
    wchar_t *value;
    enum TokenType type;
};
