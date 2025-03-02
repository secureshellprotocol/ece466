CC=gcc
LEX=flex
YACC=bison
YFLAGS=-t --language=C
CFLAGS=-g -Wall -pedantic -Iinclude/

all: objtrigger obj/grammar.o obj/ast.o obj/lex.yy.o obj/lex_utils.o obj/ast_list.o obj/astprint.o
	$(CC) $(CFLAGS) obj/*.o 

.PHONY: objtrigger
objtrigger: | obj

obj:
	mkdir -p obj

obj/grammar.o: src/parser/grammar.y
	$(YACC) \
		--header=include/parser/grammar.tab.h \
		-v --report-file=.grammar-debug.output \
		--output=src/parser/grammar.tab.c \
		--token-table \
		src/parser/grammar.y
	$(CC) $(CFLAGS) -c src/parser/grammar.tab.c -o obj/grammar.o
	
obj/ast.o: src/ast/ast.c
	$(CC) $(CFLAGS) -c src/ast/ast.c -o obj/ast.o

obj/ast_list.o: src/ast/ast_list.c
	$(CC) $(CFLAGS) -c src/ast/ast_list.c -o obj/ast_list.o

obj/astprint.o: src/ast/astprint.c
	$(CC) $(CFLAGS) -c src/ast/astprint.c -o obj/astprint.o

obj/lex.yy.o: src/lexer/lexer.lex
	$(LEX) -o src/lexer/lex.yy.c src/lexer/lexer.lex
	$(CC) $(CFLAGS) -c src/lexer/lex.yy.c -o obj/lex.yy.o
	
obj/lex_utils.o: src/lexer/lex_utils.c
	$(CC) $(CFLAGS) -c src/lexer/lex_utils.c -o obj/lex_utils.o

# === test programs ===
lex_standalone_obj:	objtrigger obj/grammar.o # builds w/ a debug header to inject a new main
	$(LEX) -o src/lexer/lex.yy.c src/lexer/lexer.lex
	$(CC) $(CFLAGS) -c src/lexer/lex.yy.c -include test/lexer/debug.h -o obj/lex.yy.o
	$(CC) $(CFLAGS) -c src/lexer/lex_utils.c -o obj/lex_utils.o
	$(CC) $(CFLAGS) obj/lex.yy.o obj/lex_utils.o -o lexer-debug.out

ast_print_test: obj/ast.o obj/astprint.o obj/lex_utils.o
	$(CC) $(CFLAGS) -c test/ast/printtest.c -o obj/ast_printtest.o 
	$(CC) $(CFLAGS) obj/ast.o obj/astprint.o obj/lex_utils.o obj/ast_printtest.o \
		-o ast_print_test.out

parser_exprtest: all 
	./a.out < test/parser/exprtests_pp.c > exprtests.output

.PHONY: clean
clean:
	rm -v -r obj/ 
	rm -v *.out *.output
