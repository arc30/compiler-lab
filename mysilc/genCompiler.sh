#!/bin/sh 
set -e
yacc -d syntaxtree.y
lex syntaxtree.l
gcc -g syntaxtree.c codegen.c symboltable.c lex.yy.c y.tab.c 
./a.out inpfile
