CC=gcc
LEX=flex
YACC=bison
YFLAGS=-t --language=C
CFLAGS=-g -Wall -pedantic -Iinclude/

all: grammar.o ast.o lex.yy.o lex_utils.o
	$(CC) $(CFLAGS) *.o 

grammar.o: src/parser/grammar.y
	$(YACC) \
		--header=include/parser/grammar.tab.h \
		-v --report-file=.grammar-debug.output \
		--output=src/parser/grammar.tab.c \
		src/parser/grammar.y
	$(CC) $(CFLAGS) -c src/parser/grammar.tab.c -o grammar.o
	
ast.o: src/parser/ast.c
	$(CC) $(CFLAGS) -c src/parser/ast.c -o ast.o

lex.yy.o: src/lexer/lexer.lex
	$(LEX) -o src/lexer/lex.yy.c src/lexer/lexer.lex
	$(CC) $(CFLAGS) -c src/lexer/lex.yy.c -o lex.yy.o
	
lex_utils.o: src/lexer/lex_utils.c
	$(CC) $(CFLAGS) -c src/lexer/lex_utils.c -o lex_utils.o

# === test programs ===
lex_standalone_obj:	grammar.o # builds w/ a debug header to inject a new main
	$(LEX) -o src/lexer/lex.yy.c src/lexer/lexer.lex
	$(CC) $(CFLAGS) -c src/lexer/lex.yy.c -include test/lexer/debug.h -o lex.yy.o
	$(CC) $(CFLAGS) -c src/lexer/lex_utils.c -o lex_utils.o
	$(CC) $(CFLAGS) lex.yy.o lex_utils.o -o lexer-debug.out

ast_print_test: ast.o lex_utils.o
	$(CC) $(CFLAGS) -c test/parser/printtest.c -o ast_printtest.o 
	$(CC) $(CFLAGS) ast.o lex_utils.o ast_printtest.o -o parser_print_test.out

clean:
	rm *.o *.out .grammar-debug.output
