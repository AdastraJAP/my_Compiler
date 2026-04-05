#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

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

typedef struct {
    const char *src;
    const char *cur;
    const char *filename;
    int line;
    TokenList *out;
} Lexer;

const char *type_name(Tokentype k) {
    switch (k) {
        case TK_INT:    return "KW:int";
        case TK_IF:     return "KW:if";
        case TK_ELSE:   return "KW:else";
        case TK_WHILE:  return "KW:while";
        case TK_RETURN: return "KW:return";
        case TK_NUMBER: return "NUMBER";
        case TK_IDENT:  return "IDENT";
        case TK_PLUS:   return "+";
        case TK_MINUS:  return "-";
        case TK_STAR:   return "*";
        case TK_SLASH:  return "/";
        case TK_EQ:     return "==";
        case TK_NEQ:    return "!=";
        case TK_LT:     return "<";
        case TK_GT:     return ">";
        case TK_LEQ:    return "<=";
        case TK_GEQ:    return ">=";
        case TK_ASSIGN: return "=";
        case TK_LPAREN: return "(";
        case TK_RPAREN: return ")";
        case TK_LBRACE: return "{";
        case TK_RBRACE: return "}";
        case TK_SEMI:   return ";";
        case TK_COMMA:  return ",";
        case TK_EOF:    return "EOF";
        default:        return "?";
    }
}

static void fatal(const char *filename, int line, const char *fmt, ...) {
    va_list args;

    fprintf(stderr, "%s:%d: ", filename, line);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");

    exit(1);
}

static void tl_init(TokenList *tl) {
    tl->cap = 64;
    tl->count = 0;
    tl->data = malloc((size_t)tl->cap * sizeof(Token));
    if (!tl->data) {
        fprintf(stderr, "fatal: out of memory\n");
        exit(1);
    }
}

static void tl_push(TokenList *tl, Token tok) {
    if (tl->count == tl->cap) {
        tl->cap *= 2;
        tl->data = realloc(tl->data, (size_t)tl->cap * sizeof(Token));
        if (!tl->data) {
            fprintf(stderr, "fatal: out of memory\n");
            exit(1);
        }
    }

    tl->data[tl->count++] = tok;
}

static void tl_free(TokenList *tl) {
    free(tl->data);
    tl->data = NULL;
    tl->count = 0;
    tl->cap = 0;
}

static int at_end(Lexer *l) {
    return *l->cur == '\0';
}

static char peek(Lexer *l) {
    return *l->cur;
}

static char advance(Lexer *l) {
    char c = *l->cur++;
    if (c == '\n') {
        l->line++;
    }
    return c;
}

static int match(Lexer *l, char expected) {
    if (at_end(l) || *l->cur != expected) {
        return 0;
    }

    l->cur++;
    return 1;
}

static void emit(Lexer *l, Tokentype type, const char *start, int len) {
    Token t;
    t.type = type;
    t.start = start;
    t.len = len;
    t.line = l->line;
    tl_push(l->out, t);
}

static Tokentype check_keyword(const char *start, int len) {
    if (len == 3 && memcmp(start, "int", 3) == 0) return TK_INT;
    if (len == 2 && memcmp(start, "if", 2) == 0) return TK_IF;
    if (len == 4 && memcmp(start, "else", 4) == 0) return TK_ELSE;
    if (len == 5 && memcmp(start, "while", 5) == 0) return TK_WHILE;
    if (len == 6 && memcmp(start, "return", 6) == 0) return TK_RETURN;
    return TK_IDENT;
}

static void scan_one(Lexer *l) {
    while (!at_end(l) && isspace((unsigned char)peek(l))) {
        advance(l);
    }

    if (at_end(l)) {
        emit(l, TK_EOF, l->cur, 0);
        return;
    }

    const char *start = l->cur;
    char c = advance(l);

    if (c == '/' && peek(l) == '/') {
        while (!at_end(l) && peek(l) != '\n') {
            advance(l);
        }
        return;
    }

    if (isdigit((unsigned char)c)) {
        while (!at_end(l) && isdigit((unsigned char)peek(l))) {
            advance(l);
        }
        emit(l, TK_NUMBER, start, (int)(l->cur - start));
        return;
    }

    if (isalpha((unsigned char)c) || c == '_') {
        while (!at_end(l) &&
               (isalnum((unsigned char)peek(l)) || peek(l) == '_')) {
            advance(l);
        }
        emit(l, check_keyword(start, (int)(l->cur - start)),
             start, (int)(l->cur - start));
        return;
    }

    switch (c) {
        case '+': emit(l, TK_PLUS, start, 1); return;
        case '-': emit(l, TK_MINUS, start, 1); return;
        case '*': emit(l, TK_STAR, start, 1); return;
        case '/': emit(l, TK_SLASH, start, 1); return;
        case '(': emit(l, TK_LPAREN, start, 1); return;
        case ')': emit(l, TK_RPAREN, start, 1); return;
        case '{': emit(l, TK_LBRACE, start, 1); return;
        case '}': emit(l, TK_RBRACE, start, 1); return;
        case ';': emit(l, TK_SEMI, start, 1); return;
        case ',': emit(l, TK_COMMA, start, 1); return;
        case '<':
            if (match(l, '=')) {
                emit(l, TK_LEQ, start, 2);
            } else {
                emit(l, TK_LT, start, 1);
            }
            return;
        case '>':
            if (match(l, '=')) {
                emit(l, TK_GEQ, start, 2);
            } else {
                emit(l, TK_GT, start, 1);
            }
            return;
        case '=':
            if (match(l, '=')) {
                emit(l, TK_EQ, start, 2);
            } else {
                emit(l, TK_ASSIGN, start, 1);
            }
            return;
        case '!':
            if (match(l, '=')) {
                emit(l, TK_NEQ, start, 2);
                return;
            }
            break;
    }

    fatal(l->filename, l->line, "unexpected character '%c'", c);
}

static TokenList lex_source(const char *src, const char *filename) {
    TokenList tl;
    Lexer l;

    tl_init(&tl);

    l.src = src;
    l.cur = src;
    l.filename = filename;
    l.line = 1;
    l.out = &tl;

    do {
        scan_one(&l);
    } while (tl.data[tl.count - 1].type != TK_EOF);

    return tl;
}

static void dump_tokens(const TokenList *tl) {
    int i;

    printf("%-6s  %-12s  %s\n", "LINE", "TYPE", "TEXT");
    printf("------  ------------  --------------------\n");

    for (i = 0; i < tl->count; i++) {
        const Token *t = &tl->data[i];
        printf("%-6d  %-12s  '%.*s'\n",
               t->line,
               type_name(t->type),
               t->len,
               t->start);
    }
}

static char *read_file(const char *path) {
    FILE *fp;
    long size;
    char *buffer;

    fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "could not open file: %s\n", path);
        exit(1);
    }

    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        fprintf(stderr, "could not seek file: %s\n", path);
        exit(1);
    }

    size = ftell(fp);
    if (size < 0) {
        fclose(fp);
        fprintf(stderr, "could not get file size: %s\n", path);
        exit(1);
    }

    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        fprintf(stderr, "could not rewind file: %s\n", path);
        exit(1);
    }

    buffer = malloc((size_t)size + 1);
    if (!buffer) {
        fclose(fp);
        fprintf(stderr, "fatal: out of memory\n");
        exit(1);
    }

    if (fread(buffer, 1, (size_t)size, fp) != (size_t)size) {
        free(buffer);
        fclose(fp);
        fprintf(stderr, "could not read file: %s\n", path);
        exit(1);
    }

    buffer[size] = '\0';
    fclose(fp);
    return buffer;
}

int main(int argc, char **argv) {
    char *source;
    TokenList tokens;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <source-file>\n", argv[0]);
        return 1;
    }

    source = read_file(argv[1]);
    tokens = lex_source(source, argv[1]);
    dump_tokens(&tokens);

    tl_free(&tokens);
    free(source);
    return 0;
}
