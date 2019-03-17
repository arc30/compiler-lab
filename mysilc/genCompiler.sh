#!/bin/sh 
set -e
yacc -d syntaxtree.y
lex syntaxtree.l
lex -P lt labeltrans.l
gcc -g lex.yy.c lex.lt.c y.tab.c syntaxtree.c symboltable.c funcdef.c typetable.c 

./a.out inpfile


