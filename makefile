CC     = gcc
CFLAGS = -std=c11 -Wall -Wextra -g -Iinclude

all:
	$(CC) $(CFLAGS) -o myCompiler src/main.c src/lexer.c

clean:
	del myCompiler.exe


	
