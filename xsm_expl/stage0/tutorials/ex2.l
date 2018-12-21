%option noyywrap
%{
	#include <stdlib.h>
	#include <stdio.h>
	int printEvenOrOdd(int num);
%}
	
number [0-9]+
 
%%
 
{number} {	return printEvenOrOdd(atoi(yytext));	}
 
%%
 
int main()
{
	yylex();
	
	return 1;
}

int printEvenOrOdd(int num)
{
	if(num%2)
		printf("ODD\n");
	else
		printf("EVEN\n");
	return num;
}
