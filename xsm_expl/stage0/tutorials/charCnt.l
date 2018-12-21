%{


#include<stdio.h>
int wordCount=0, lineCount=0, charCount=0;
int getCharCount();

%}

low    [a-z]
upp    [A-Z]
number [0-9]

%option noyywrap

%%

['\n']	{lineCount+=1; wordCount+=1;}
[' ']	{wordCount+=1;}
[a-zA-Z]+	{charCount+=getCharCount();}




%%

int getCharCount()
{
	return yyleng;
}


int main()
{
	int input = yylex();
	//charCount=getCharCount();
	printf("No of chars: %d\n", charCount);
	printf("No of words: %d\n", wordCount);
	printf("No of lines: %d\n", lineCount);

	return 1;
}

