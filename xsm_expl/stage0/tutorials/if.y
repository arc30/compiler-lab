%{

#include <stdio.h>
#include <stdlib.h>
int levelCount=0;
%}

%token ID RELOP S NUM IF

%%

stmt : if_stmt {printf("levels %d", levelCount);}

if_stmt : IF '(' cond ')' if_stmt{levelCOunt++;}
		| S;
		
cond : x RELOP x;

x	: ID
	| NUM
	;
	
%%

int yyerror(char *msg)
{
	 printf(“Invalid Expression\n”);
	 exit(0);
}
main ()
{
	 printf(“Enter the statement”);
	 yyparse();
} 
