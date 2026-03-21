CC     = gcc
CFLAGS = -std=c11 -Wall -Wextra -g

all:
	$(CC) $(CFLAGS) -o myCompiler src/Tokenizer.c

clean:
	del myCompiler.exe


	