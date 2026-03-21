#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>


typedef enum{
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

}Tokentype;


typedef struct{
 Tokentype type;
 const char *start;
 int len;
 int line;

}Token;

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

typedef struct {
    const char *src;    
    const char *cur;    
    int         line;   
} Lexer;



 static char peek(Lexer *l) {
    return *l->cur;
}

static char advance(Lexer *l) {
    char c = *l->cur++;
    if (c == '\n') l->line++;
    return c;
}

static int match(Lexer *l, char expected) {
    if (*l->cur != expected) return 0;
    l->cur++;
    return 1;
}





static Tokentype check_keyword(const char *start, int len) {
    if (len == 3 && memcmp(start, "int",    3) == 0) return TK_INT;
    if (len == 2 && memcmp(start, "if",     2) == 0) return TK_IF;
    if (len == 4 && memcmp(start, "else",   4) == 0) return TK_ELSE;
    if (len == 5 && memcmp(start, "while",  5) == 0) return TK_WHILE;
    if (len == 6 && memcmp(start, "return", 6) == 0) return TK_RETURN;
    return TK_IDENT;
}


static Token next_token(Lexer *l) {
    
    while (*l->cur && isspace((unsigned char)*l->cur))
        advance(l);

    Token t;
    t.start = l->cur;
    t.line  = l->line;
    t.len   = 1;

    
    if (*l->cur == '\0') {
        t.type = TK_EOF;
        t.len  = 0;
        return t;
    }

    char c = advance(l);

    
    if (isdigit((unsigned char)c)) {
        while (isdigit((unsigned char)*l->cur))
            advance(l);
        t.type = TK_NUMBER;
        t.len  = (int)(l->cur - t.start);
        return t;
    }

    
    if (isalpha((unsigned char)c) || c == '_') {
        while (isalnum((unsigned char)*l->cur) || *l->cur == '_')
            advance(l);
        t.len  = (int)(l->cur - t.start);
        t.type = check_keyword(t.start, t.len);
        return t;
    }

    
    if (c == '/' && peek(l) == '/') {
        while (*l->cur && *l->cur != '\n')
            advance(l);
        return next_token(l);
    }

    
    switch (c) {
        case '+': t.type = TK_PLUS;   return t;
        case '-': t.type = TK_MINUS;  return t;
        case '*': t.type = TK_STAR;   return t;
        case '/': t.type = TK_SLASH;  return t;
        case '(': t.type = TK_LPAREN; return t;
        case ')': t.type = TK_RPAREN; return t;
        case '{': t.type = TK_LBRACE; return t;
        case '}': t.type = TK_RBRACE; return t;
        case ';': t.type = TK_SEMI;   return t;
        case ',': t.type = TK_COMMA;  return t;
        case '<':
            if (match(l, '=')) { t.type = TK_LEQ; t.len = 2; }
            else                  t.type = TK_LT;
            return t;
        case '>':
            if (match(l, '=')) { t.type = TK_GEQ; t.len = 2; }
            else                  t.type = TK_GT;
            return t;
        case '=':
            if (match(l, '=')) { t.type= TK_EQ;     t.len = 2; }
            else                  t.type = TK_ASSIGN;
            return t;
        case '!':
            if (match(l, '=')) { t.type= TK_NEQ; t.len = 2; return t; }
            break;
    }

    
    fprintf(stderr, "line %d: unexpected character '%c'\n", t.line, c);
    return next_token(l);
}










int main(){
     const char *source =
        "int add(int a, int b) {\n"
        "    return a + b;\n"
        "}\n"
        "\n"
        "int main() {\n"
        "    int x = add(3, 7);\n"
        "    if (x > 5) {\n"
        "        return x;\n"
        "    }\n"
        "    return 0;\n"
        "}\n";

    Lexer l;
    l.src  = source;
    l.cur  = source;
    l.line = 1;

    printf("%-6s  %-12s  %s\n", "LINE", "TYPE", "TEXT");
    printf("------  ------------  --------------------\n");

    Token t;
    do {
        t = next_token(&l);
        printf("%-6d  %-12s  '%.*s'\n",
               t.line,
               type_name(t.type),
               t.len,
               t.start);
    } while (t.type != TK_EOF);

    return 0;
}




