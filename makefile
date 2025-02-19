CC=gcc
LEX=flex
YACC=bison
CFLAGS=-Wall -pedantic
SRC=src

all: lex_obj

lex_obj: src/lexer/lex.yy.c src/lexer/lex_utils.c
	$(LEX) -o src/lexer/lex.yy.c src/lexer/lexer.lex
	$(CC) $(CFLAGS) -c src/lexer/lex.yy.c -o lex.yy.o
	$(CC) $(CFLAGS) -c src/lexer/lex_utils.c -o lex_utils.o

lex_standalone_obj:
	$(LEX) -o src/lexer/lex.yy.c src/lexer/lexer.lex
	$(CC) $(CFLAGS) -c src/lexer/lex.yy.c -include test/lexer/debug.h -o lex.yy.o
	$(CC) $(CFLAGS) -c src/lexer/lex_utils.c -o lex_utils.o

lex_standalone: lex_standalone_obj
	$(CC) $(CFLAGS) lex.yy.o lex_utils.o -o lexer

clean:
	rm *.o lexer
