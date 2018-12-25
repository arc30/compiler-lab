#!/bin/sh 
set -e
yacc -d exptree.y
lex exptree.l
gcc lex.yy.c y.tab.c 
./a.out 
