#include <stdio.h>
#include <stdlib.h>

#include "../include/lexer.h"

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
    tokens = lex(source, argv[1]);
    dump_tokens(&tokens);

    token_list_free(&tokens);
    free(source);
    return 0;
}
