CC=gcc
LEX=flex
YACC=bison
YFLAGS=-t --language=C
CFLAGS=-g3 \
	   -Wall -Wpedantic -Wimplicit-fallthrough \
	   -Iinclude/
OUTNAME=ccc

all: \
	objtrigger obj/main.o \
	obj/grammar.o \
	obj/lex.yy.o obj/lex_utils.o \
	obj/ast.o obj/ast_list.o obj/astprint.o obj/types.o obj/selection.o obj/ast_utils.o \
	obj/symtab.o obj/symtabprint.o \
	obj/basicblocks.o obj/bb_args.o obj/bb_ops.o obj/bbprinter.o
	$(CC) $(CFLAGS) obj/*.o -o $(OUTNAME)

.PHONY: objtrigger
objtrigger: | obj

obj:
	mkdir -p obj

obj/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o obj/main.o

obj/grammar.o: src/parser/grammar.y
	$(YACC) \
		--header=include/parser/grammar.tab.h \
		-v --report-file=.grammar-debug.output \
		--output=src/parser/grammar.tab.c \
		--token-table \
		-Wcex \
		src/parser/grammar.y
	$(CC) $(CFLAGS) -c src/parser/grammar.tab.c -o obj/grammar.o

obj/lex.yy.o: src/lexer/lexer.lex
	$(LEX) -o src/lexer/lex.yy.c src/lexer/lexer.lex
	$(CC) $(CFLAGS) -c src/lexer/lex.yy.c -o obj/lex.yy.o
	
obj/lex_utils.o: src/lexer/lex_utils.c
	$(CC) $(CFLAGS) -c src/lexer/lex_utils.c -o obj/lex_utils.o

obj/ast.o: src/ast/ast.c
	$(CC) $(CFLAGS) -c src/ast/ast.c -o obj/ast.o

obj/types.o: src/ast/types.c
	$(CC) $(CFLAGS) -c src/ast/types.c -o obj/types.o

obj/selection.o: src/ast/selection.c
	$(CC) $(CFLAGS) -c src/ast/selection.c -o obj/selection.o

obj/ast_list.o: src/ast/ast_list.c
	$(CC) $(CFLAGS) -c src/ast/ast_list.c -o obj/ast_list.o

obj/ast_utils.o: src/ast/ast_utils.c
	$(CC) $(CFLAGS) -c src/ast/ast_utils.c -o obj/ast_utils.o

obj/astprint.o: src/ast/astprint.c
	$(CC) $(CFLAGS) -c src/ast/astprint.c -o obj/astprint.o

obj/symtab.o: src/symtab/symtab.c
	$(CC) $(CFLAGS) -c src/symtab/symtab.c -o obj/symtab.o

obj/symtabprint.o: src/symtab/symtabprint.c
	$(CC) $(CFLAGS) -c src/symtab/symtabprint.c -o obj/symtabprint.o

obj/basicblocks.o: src/backend/basicblocks.c
	$(CC) $(CFLAGS) -c src/backend/basicblocks.c -o obj/basicblocks.o 

obj/bb_args.o: src/backend/bb_args.o
	$(CC) $(CFLAGS) -c src/backend/bb_args.c -o obj/bb_args.o

obj/bb_ops.o: src/backend/bb_ops.o
	$(CC) $(CFLAGS) -c src/backend/bb_ops.c -o obj/bb_ops.o

obj/bbprinter.o: src/backend/bbprinter.o
	$(CC) $(CFLAGS) -c src/backend/bbprinter.c -o obj/bbprinter.o

# === test programs ===
lex_standalone_obj:	objtrigger obj/grammar.o # builds w/ a debug header to inject a new main
	$(LEX) -o src/lexer/lex.yy.c src/lexer/lexer.lex
	$(CC) $(CFLAGS) -c src/lexer/lex.yy.c -include test/lexer/debug.h \
		-o obj/lex.yy.o
	$(CC) $(CFLAGS) -c src/lexer/lex_utils.c -o obj/lex_utils.o
	$(CC) $(CFLAGS) obj/lex.yy.o obj/lex_utils.o -o lexer-debug.out

ast_print_test: obj/ast.o obj/astprint.o obj/lex_utils.o
	$(CC) $(CFLAGS) -c test/ast/printtest.c -o obj/ast_printtest.o 
	$(CC) $(CFLAGS) obj/ast.o obj/astprint.o obj/lex_utils.o obj/ast_printtest.o \
		-o ast_print_test.out

vector_test: obj/ast.o obj/astprint.o obj/vector.o obj/lex_utils.o
	$(CC) $(CFLAGS) -c test/symtab/vectortester.c -o obj/vectortest.o
	$(CC) $(CFLAGS) obj/ast.o obj/astprint.o obj/vectortest.o obj/vector.o \
		obj/lex_utils.o \
		-o vector_test.out

parser_exprtest: all 
	./a.out < test/parser/exprtests_pp.c > exprtests.output

.PHONY: clean
clean:
	rm -v -r obj/ 
	rm -v *.out *.output $(OUTNAME)
