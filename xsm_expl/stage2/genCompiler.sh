#!/bin/sh 
set -e
yacc -d syntaxtree.y
lex syntaxtree.l
gcc syntaxtree.c codegen.c lex.yy.c y.tab.c 
./a.out inpfile
