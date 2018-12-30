

%{

	#include <stdio.h>
	#include <stdlib.h>

	

	#include "syntaxtree.h"

	#define YYSTYPE tnode *
	
	extern int yylex();
	extern int yyparse();
	extern FILE *yyin;
	
%}
	
	%token OPERATOR NUM ID END BEG CONNECTOR READ WRITE
	%token PLUS MINUS MUL DIV ASSGN
	%left PLUS MINUS
	%left MUL DIV
	%right ASSGN
	%%
	
	program : BEG slist END 
	{		
		printf("Generating AST, inorderForm is: \n");
		inorderForm($2);
		exit(1);
	}
	|BEG END {printf("No statements\n"); exit(1); }	
	;
	
	slist : slist stmt { $$=makeConnectorNode(CONNECTOR,$1,$2); }
	| stmt	{ $$ = $1;	}
	;
	
	stmt : inputstmt { $$=$1;}
	| outputstmt { $$=$1; }
	stmt : assgnstmt {$$=$1;}
	;
	
	inputstmt : READ '(' ID ')' ';'	{$$ = makeReadNode(READ, $3);}

	outputstmt : WRITE '(' expr ')' ';' {$$ = makeWriteNode(WRITE, $3);}
		
	assgnstmt : ID ASSGN expr ';' { $$ = makeAssignmentNode(ASSGN,'=',$1,$3); }
	
	expr : expr PLUS expr {$$ = makeOperatorNode(PLUS,'+',$1,$3);}
	| expr MINUS expr {$$ = makeOperatorNode(MINUS, '-',$1,$3);}
	| expr MUL expr {$$ = makeOperatorNode(MUL, '*',$1,$3);}
	| expr DIV expr {$$ = makeOperatorNode(DIV, '/',$1,$3);}
	| '(' expr ')' {$$ = $2;}
	| NUM {$$ = $1;}
	| ID {$$ = $1;}
	;
	
	
	
	%%
	
	yyerror(char const *s)
	{
	printf("yyerror %s",s);
	}
	
	
int main(int argc, char** argv) 
{
	if(argc == 2)
	{
		FILE *fp = fopen(argv[1], "r");
		if(fp)
		{
			yyin = fp;
		}
	}
	else
	{
		printf("Required format: ./a.out <input filename> ");
		return 0;
	}
	yyparse();
	return 0;
	}


