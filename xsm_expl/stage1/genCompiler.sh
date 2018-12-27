#!/bin/sh 
set -e
yacc -d exptree.y
lex exptree.l
gcc exptree.c codegen.c lex.yy.c y.tab.c 
./a.out 
