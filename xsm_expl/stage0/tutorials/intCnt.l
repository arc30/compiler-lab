%{


#include<stdio.h>
int intCount=0, floatCount=0;


%}


number [0-9]
numbers	{number}+
integer	[-]?{numbers}
floatingPoint	{integer}?[.]{numbers}


%option noyywrap

%%

{integer}	{intCount++;}
{floatingPoint}	{floatCount++;}




%%


int main()
{
	int input = yylex();
	//charCount=getCharCount();
	printf("No of ints: %d	No. of floats: %d", intCount, floatCount);
	return 1;
}

