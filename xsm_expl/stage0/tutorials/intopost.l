%{
#include <stdio.h>
#include "y.tab.h"
%}

%%

[0-9]+	{ yylval=yytext;
		  return DIGIT;
		}
		
[a-zA-Z]+	{
			yylval=yytext;			
			return SINGLECHAR;
			}
			
"+"		return *yytext;
"-"		return *yytext;
"*"		return *yytext;
[()]	return *yytext;
[\n]	return NEWLINE;

%%

yywrap()
{
	return 1;
}
