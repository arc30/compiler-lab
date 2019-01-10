

%{

	#include <stdio.h>
	#include <stdlib.h>

	#include "codegen.h"

	#include "syntaxtree.h"

	#define YYSTYPE tnode *
	
	extern int yylex();
	extern int yyparse();
	extern FILE *yyin;
	
%}
	
	%token OPERATOR NUM ID END BEG CONNECTOR READ WRITE 
	%token IF THEN ELSE ENDIF WHILE DO ENDWHILE IFELSE ASSGN

	%left EQUAL NOTEQUAL
	%left GREATERTHAN GREATERTHAN_EQUAL LESSTHAN LESSTHAN_EQUAL
	%left PLUS MINUS
	%left MUL DIV
	%%
	
	program : BEG slist END 
	{		
		printf("Generating AST, inorderForm is: \n");
		inorderForm($2);
		printf("Calling codegen \n");
		FILE *fptr = fopen("targetfile.xsm","w");
		codeGenXsm($2, fptr);
		exit(1);
	}
	|BEG END {printf("No statements\n"); exit(1); }	
	
	;
	
	slist : slist stmt { $$=makeConnectorNode(CONNECTOR,$1,$2); }
	| stmt	{ $$ = $1;	}
	;
	
	stmt : inputstmt { $$=$1;}
		| outputstmt { $$=$1; }
		| assgnstmt {$$=$1;}
		| ifstmt 	{ $$=$1; }
		|whilestmt { $$=$1; }
	;
	
	inputstmt : READ '(' ID ')' ';'	{$$ = makeReadNode(READ, $3);}

	outputstmt : WRITE '(' expr ')' ';' {$$ = makeWriteNode(WRITE, $3);}
		
	assgnstmt : ID ASSGN expr ';' { $$ = makeAssignmentNode(ASSGN,'=',$1,$3); }
	
	ifstmt : IF '(' expr ')' THEN slist ELSE slist ENDIF ';'
				{
					$$ = makeIfThenElseNode(IFELSE,$3,$6,$8);
				}
			| IF '(' expr ')' THEN slist ENDIF ';'
				{
					$$ = makeIfThenNode(IF,$3,$6);
				}
			;
	
	whilestmt : WHILE '(' expr ')' DO slist ENDWHILE ';'
				{
					$$ = makeWhileNode(WHILE, $3, $6);
				}
			;


	expr : expr PLUS expr 
			{
				$$ = makeOperatorNode(PLUS,inttype,$1,$3);
			}
	| expr MINUS expr 
			{
				$$ = makeOperatorNode(MINUS, inttype,$1,$3);
			}
	| expr MUL expr 
			{
				$$ = makeOperatorNode(MUL,inttype,$1,$3);
			}
	| expr DIV expr 
			{
				$$ = makeOperatorNode(DIV, inttype,$1,$3);
			}
	| '(' expr ')' 
			{
				$$ = $2;
				//$$->type = $2->type;
			}	
	| NUM {$$ = $1; }
	| ID 
		{
			$$ = $1;
		
		}
	| expr GREATERTHAN expr
		{
			$$ = makeOperatorNode(GREATERTHAN, booltype, $1, $3);

		}

	| expr LESSTHAN expr
		{
			$$ = makeOperatorNode(LESSTHAN, booltype, $1, $3);
		}

	| expr GREATERTHAN_EQUAL expr
		{
			$$ = makeOperatorNode(GREATERTHAN_EQUAL, booltype, $1, $3);
		}

	| expr LESSTHAN_EQUAL expr
		{
			$$ = makeOperatorNode(LESSTHAN_EQUAL, booltype, $1, $3);
		}

	|expr EQUAL expr
		{
			$$ = makeOperatorNode(EQUAL, booltype, $1, $3);
		}

	|expr NOTEQUAL expr
		{
			$$ = makeOperatorNode(NOTEQUAL,booltype, $1, $3);
		}
		
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


