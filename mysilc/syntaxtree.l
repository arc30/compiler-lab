%{
	#include <stdio.h>
	#include "y.tab.h"
	#include <stdlib.h>
	#include "syntaxtree.h"
	#include <string.h>

	int number;
	char* varname;
	
%}
	
%%
	
[0-9]+ {number = atoi(yytext); yylval = makeLeafNodeNum(NUM, number); return NUM;}

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
[();,] {return *yytext;}
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
"continue"	{return CONTINUE;}
"repeat"	{return REPEAT;}
"until"		{return UNTIL;}

"decl"		{return DECL;}
"enddecl"	{return ENDDECL;}
"int"		{return INT;}
"str"		{return STR;}

[a-z]+	{varname = (char*) malloc(yyleng+1);
		strncpy(varname, yytext, yyleng);
		yylval = makeLeafNodeVar(ID, varname);	
		return ID;}

. {yyerror("unknown character\n");exit(1);}
	
%%
	
	int yywrap(void) 
	{
	return 1;
	}
	
