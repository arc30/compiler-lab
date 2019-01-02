%{
	#include <stdio.h>
	#include "y.tab.h"
	#include <stdlib.h>

	int number;
	char varname;
	
%}
	
%%
	
[0-9]+ {number = atoi(yytext); yylval = makeLeafNodeNum(NUM, number); return NUM;}
[a-z]	{varname = *yytext; yylval = makeLeafNodeVar(ID, varname);	return ID;}
"+" {return PLUS;}
"-" {return MINUS;}
"*" {return MUL;}
"/" {return DIV;}
"=" {return ASSGN;}
[ \t\n] {}
[();] {return *yytext;}
"begin" {return BEG;}
"end"	{return END;}
"read" {return READ;}
"write" {return WRITE;}

. {yyerror("unknown character\n");exit(1);}
	
%%
	
	int yywrap(void) 
	{
	return 1;
	}
	
