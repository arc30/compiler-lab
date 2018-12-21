%{


#include<stdio.h>
int A3Count=0;


%}


number [0-9]
alpha [a-zA-Z]
nonalpha [^{alpha}]

%option noyywrap

%%


[A]{alpha}{alpha}[' ' '\n']	{A3Count++;}



%%


int main()
{
	int input = yylex();
	//charCount=getCharCount();
	printf("No of 3len words starting with A: %d \n", A3Count); 
	return 1;
}

