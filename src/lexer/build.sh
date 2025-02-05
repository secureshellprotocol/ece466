#!/bin/bash
flex lexer.lex && gcc -g lex.yy.c lex_utils.c
