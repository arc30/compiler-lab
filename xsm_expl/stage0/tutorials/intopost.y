%{
#include <stdio.h>

%}

%token DIGIT NEWLINE SINGLECHAR
%left '+' '-'
%left '*'
//%left '('  ')'

%%

start : expr NEWLINE	{printf("We're done !\n"); exit(1);}
	  ;	
	  
expr : expr '+' expr		{printf("PLUS ");}
	 | expr '-' expr		{printf("MINUS ");}
	 | expr '*' expr		{printf("MUL ");}
	 | '(' expr ')'			
	 | DIGIT				{printf("%s ",$1);}
	 | SINGLECHAR			{printf("%s ",$1);}
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
