#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TK_INT,
    TK_IF,
    TK_ELSE,
    TK_WHILE,
    TK_RETURN,

    TK_NUMBER,
    TK_IDENT,

    TK_PLUS,
    TK_MINUS,
    TK_STAR,
    TK_SLASH,

    TK_EQ,
    TK_NEQ,
    TK_LT,
    TK_GT,
    TK_LEQ,
    TK_GEQ,
    TK_ASSIGN,

    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACE,
    TK_RBRACE,
    TK_SEMI,
    TK_COMMA,

    TK_EOF
} Tokentype;

typedef struct {
    Tokentype type;
    const char *start;
    int len;
    int line;
} Token;

typedef struct {
    Token *data;
    int count;
    int cap;
} TokenList;

TokenList lex(const char *src, const char *filename);
void token_list_free(TokenList *tl);
void dump_tokens(const TokenList *tl);
const char *type_name(Tokentype k);

#endif
