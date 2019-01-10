%{
	#include <stdio.h>
	#include "y.tab.h"
	#include <stdlib.h>
	#include "syntaxtree.h"

	int number;
	char varname;
	
%}
	
%%
	
[0-9]+ {number = atoi(yytext); yylval = makeLeafNodeNum(NUM, number, inttype); return NUM;}
[a-z]	{varname = *yytext; yylval = makeLeafNodeVar(ID, varname, inttype);	return ID;}
"+" {return PLUS;}
"-" {return MINUS;}
"*" {return MUL;}
"/" {return DIV;}
"=" {return ASSGN;}
"==" {return EQUAL;}
">=" {return GREATERTHAN_EQUAL;}
"<=" {return LESSTHAN_EQUAL;}
">"	{return GREATERTHAN;}
"<"	{return LESSTHAN;}
"!=" {return NOTEQUAL;}
[ \t\n] {}
[();] {return *yytext;}
"begin" {return BEG;}
"end"	{return END;}
"read" {return READ;}
"Read" {return READ;}

"write" {return WRITE;}
"Write" {return WRITE;}

"if"	{return IF;}
"then"	{return THEN;}
"else"	{return ELSE;}
"endif"	{return ENDIF;}
"while"	{return WHILE;}
"do"	{return DO;}
"endwhile"	{return ENDWHILE;}
"break"	{return BREAK;}


. {yyerror("unknown character\n");exit(1);}
	
%%
	
	int yywrap(void) 
	{
	return 1;
	}
	
