

%{

	#include <stdio.h>
	#include <stdlib.h>

	#include "codegen.h"

	#include "syntaxtree.h"

	#include "symboltable.h"

	#define YYSTYPE tnode *
	
	extern int yylex();
	extern int yyparse();
	extern FILE *yyin;

	int currType = NOTYPE;
	
%}

%error-verbose
	
	%token OPERATOR NUM ID END BEG CONNECTOR READ WRITE 
	%token IF THEN ELSE ENDIF WHILE DO ENDWHILE IFELSE ASSGN BREAK CONTINUE
	%token REPEAT UNTIL
	%token DECL ENDDECL INT STR STRCONST


	%left EQUAL NOTEQUAL
	%left GREATERTHAN GREATERTHAN_EQUAL LESSTHAN LESSTHAN_EQUAL
	%left PLUS MINUS
	%left MUL DIV
%%
	
	
	
	start : declarations program	
		   ;

	declarations : DECL declist ENDDECL		{printf("declaration list\n");
											printSymbolTable();	
											}
				|	DECL ENDDECL			{printf("No decl list\n") ; }
				;

	declist : declist declr					{}			
			|declr							{}
			;

	declr	:  type varlist ';'				{}
			;

	type	: INT						{ currType = INTTYPE; }
			| STR						{ currType = STRTYPE; }
			;

	varlist	: varlist ',' ID			{
										 install($3->varname, currType, 1); 
										}
			| ID						{
										 install($1->varname, currType, 1);
										}
			| varlist ',' ID '[' NUM ']' {
										 install($3->varname, currType, $5->val);
										 }
			| ID '[' NUM ']'			{
										 install($1->varname, currType, $3->val);
										}
			;

	

	program : BEG slist END 
	{		
		printf("Generating AST, inorderForm is \n");
		inorderForm($2);

		printf("\n\nCalling codegen \n");
		FILE *fptr = fopen("targetfile.xsm","w");
		codeGenXsm($2, fptr);
		printf("Finished CodeGen\n");

	}
	|	BEG END {printf("No statements\n");  }	
	;


	


	slist : slist stmt { $$=makeConnectorNode(CONNECTOR,$1,$2); }
	| stmt	{ $$ = $1;	}
	;
	
	stmt : inputstmt { $$=$1;}
		| outputstmt { $$=$1; }
		| assgnstmt {$$=$1;}
		| ifstmt 	{ $$=$1; }
		| whilestmt { $$=$1; }
		| repeatuntilstmt	{$$ =$1;}
		| breakstmt {$$=$1;}
		| continuestmt	{$$=$1;}
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
	
	repeatuntilstmt : REPEAT slist UNTIL '(' expr ')' ';'
				{
					$$ = makeRepeatNode(REPEAT, $2, $5 );
				}
			;
			
	breakstmt : BREAK ';'
				{
					$$ = makeBreakNode(BREAK);
				}
			;

	continuestmt	:	CONTINUE ';'
				{
					$$ = makeContinueNode(CONTINUE);
				}
			;

	expr : expr PLUS expr 
			{
				$$ = makeOperatorNode(PLUS,INTTYPE,$1,$3);
			}
	| expr MINUS expr 
			{
				$$ = makeOperatorNode(MINUS, INTTYPE,$1,$3);
			}
	| expr MUL expr 
			{
				$$ = makeOperatorNode(MUL,INTTYPE,$1,$3);
			}
	| expr DIV expr 
			{
				$$ = makeOperatorNode(DIV, INTTYPE,$1,$3);
			}
	| '(' expr ')' 
			{
				$$ = $2;
			}	
	| NUM {$$ = $1; }
	| ID 
		{
			$$ = $1;
		
		}

	| STRCONST {$$ = $1;}	

	| expr GREATERTHAN expr
		{
			$$ = makeOperatorNode(GREATERTHAN, BOOLTYPE, $1, $3);

		}

	| expr LESSTHAN expr
		{
			$$ = makeOperatorNode(LESSTHAN, BOOLTYPE, $1, $3);
		}

	| expr GREATERTHAN_EQUAL expr
		{
			$$ = makeOperatorNode(GREATERTHAN_EQUAL, BOOLTYPE, $1, $3);
		}

	| expr LESSTHAN_EQUAL expr
		{
			$$ = makeOperatorNode(LESSTHAN_EQUAL, BOOLTYPE, $1, $3);
		}

	|expr EQUAL expr
		{
			$$ = makeOperatorNode(EQUAL, BOOLTYPE, $1, $3);
		}

	|expr NOTEQUAL expr
		{
			$$ = makeOperatorNode(NOTEQUAL,BOOLTYPE, $1, $3);
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
		printf("Required : ./a.out <input filename>") ;
		return 0;
		
	}
	yyparse();
	return 0;
}


