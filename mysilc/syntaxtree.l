%{
	#include <stdio.h>
	#include "y.tab.h"
	#include <stdlib.h>
	#include "syntaxtree.h"
	#include <string.h>

	int number;
	char* varname;
	char* constname;
	void yyerror(char const *s);

	
%}
	
%%
	
[0-9]+ {number = atoi(yytext); yylval = makeLeafNodeNum(NUM, number); return NUM;}

"+" {return PLUS;}
"-" {return MINUS;}
"*" {return MUL;}
"/" {return DIV;}
"&" {return ADDROF;}
"=" {return ASSGN;}
"==" {return EQUAL;}
">=" {return GREATERTHAN_EQUAL;}
"<=" {return LESSTHAN_EQUAL;}
">"	{return GREATERTHAN;}
"<"	{return LESSTHAN;}
"!=" {return NOTEQUAL;}
"%"  {return MOD;}
[ \t\n] {}
"["	{return *yytext;}
"]"	{return *yytext;}
[();,.{}] {return *yytext;}
"begin" {return BEG;}
"end"	{return END;}
"type"	{return TYPE;}
"endtype" {return ENDTYPE;}	
"read" {return READ;}
"null"	{return NULLCONST;}
"write" {return WRITE;}

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
"return"	{return RETURN;}
"until"		{return UNTIL;}

"decl"		{return DECL;}
"enddecl"	{return ENDDECL;}
"int"		{return INT;}
"str"		{return STR;}
"main"		{return MAIN;}


["][^"]*["]	{
			constname = (char*) malloc(yyleng+1);
			strncpy(constname, yytext+1, yyleng-2);
			yylval = makeLeafNodeStringConst(STRCONST, constname);
	
			return STRCONST;}

[_a-zA-Z][A-Za-z_0-9]*	{varname = (char*) malloc(yyleng+1);
		strncpy(varname, yytext, yyleng);
		yylval = makeLeafNodeVar(ID, varname);	
		return ID;}

. {printf("%s",yytext);yyerror("unknown character\n"); exit(1);}
	
%%
	
	int yywrap(void) 
	{
	return 1;
	}
	
