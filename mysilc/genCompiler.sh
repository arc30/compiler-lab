#!/bin/sh 
set -e
yacc -d syntaxtree.y
lex syntaxtree.l

gcc -g lex.yy.c y.tab.c syntaxtree.c codegen.c symboltable.c 

./a.out inpfile
