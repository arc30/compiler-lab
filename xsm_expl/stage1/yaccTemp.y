%{
#include <stdio.h>

%}

%token DIGIT NEWLINE 
%left '+' '-'


%%

start : expr NEWLINE	{printf("We're done !\n"); exit(1);}
	  ;	
	  
 ;
	 
%%

void yyerror(char const *s)
{
    printf("yyerror  %s\n",s);
    return ;
}

int main()
{
  yyparse();
  return 1;
}
