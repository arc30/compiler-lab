

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
	extern FILE *ltin;

	int currType = NOTYPE;
	
%}

%error-verbose
	
	%token OPERATOR NUM ID END BEG CONNECTOR READ WRITE 
	%token IF THEN ELSE ENDIF WHILE DO ENDWHILE IFELSE ASSGN BREAK CONTINUE
	%token REPEAT UNTIL
	%token DECL ENDDECL INT STR STRCONST
	%token ARR ARR2D DEREF 

	%token INTPTR STRPTR

	%left EQUAL NOTEQUAL
	%left GREATERTHAN GREATERTHAN_EQUAL LESSTHAN LESSTHAN_EQUAL
	%left PLUS MINUS
	%left MUL DIV MOD
	%right ADDROF
%%
	
	
	
	program : GDeclBlock codeSection	
		   ;

	GDeclBlock : DECL Gdeclist ENDDECL		{printf("global declaration list\n");
											printSymbolTable();	
											}
				|	DECL ENDDECL			{printf("No global decl list\n") ; }
				;

	Gdeclist : Gdeclist Gdeclr					{}			
			|Gdeclr							{}
			;

	Gdeclr	:  type varlist ';'				{}
			;

	type	: INT						{ currType = INTTYPE; }
			| STR						{ currType = STRTYPE; }
			;

	varlist : varlist ',' var				
			| var
			;


	
	var : 		ID						{
										 install($1->varname, currType, 1, 0);
										}
			| ID '[' NUM ']'			{
										 install($1->varname, currType, $3->val, 0);
										}
			| ID '[' NUM ']' '[' NUM ']'{
										install($1->varname, currType, ($3->val)*($6->val), $6->val);
										}
			| MUL ID					{	//same as *ID

											if(currType == INTTYPE)
											{
												install($2->varname, INTPTR, 1, 0);
											}
											else if(currType == STRTYPE)
											{
												install($2->varname, STRPTR, 1, 0);
											}
											else
											{
												printf("%d Error *id", currType); exit(1);
											}
											
										}
			| ID '(' paramlist ')'		{
											//install function in gsymboltable

										}

			;

	paramlist : paramlist, param
			  |	
			  ;
	param : type ID
		  ;
	

	codeSection : BEG slist END 
	{		

		char* file1="targetfile.xsm";

		printf("Generating AST, inorderForm is \n");
		inorderForm($2);
	
		printf("\n\nCalling codegen \n");
		FILE *fptr = fopen(file1,"w");
		codeGenXsm($2, fptr);
		printf("Finished CodeGen\n");

		fclose(fptr); //IMP

		ltin = fopen(file1,"r");
		ltlex();

		fclose(ltin);
		
		return 1;


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
			  | READ '(' ID '[' expr ']' ')' ';' { $$ = makeReadNode(READ, makeArrayNode(ARR,$3,$5)); }
			  | READ '(' ID '[' expr ']' '[' expr ']' ')' ';' { $$ = makeReadNode(READ, make2DArrayNode(ARR2D,$3,$5, $8)); }

	outputstmt : WRITE '(' expr ')' ';' {$$ = makeWriteNode(WRITE, $3);}
		
	assgnstmt : ID ASSGN expr ';' { $$ = makeAssignmentNode(ASSGN,'=',$1,$3); }

			  | ID '[' expr ']' ASSGN expr ';'	
			  	{ $$ = makeAssignmentNode(ASSGN,'=', makeArrayNode(ARR,$1,$3), $6);	}

			  | ID '[' expr ']' '[' expr ']' ASSGN expr ';'
			  	{ $$ = makeAssignmentNode(ASSGN,'=', make2DArrayNode(ARR2D,$1,$3, $6), $9); }

			  | MUL ID ASSGN expr ';'	{ $$ = makeAssignmentNode(ASSGN,'=', makeOperatorNode(DEREF, $2->gEntry->type, NULL, $2 ), $4 );	}
			  ;	

	
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

	expr : MUL ID
			{
				$$ = makeOperatorNode(DEREF, $2->gEntry->type, NULL, $2 );
			}
	| ADDROF ID
			{
				$$ = makeOperatorNode(ADDROF, $2->gEntry->type, NULL, $2);
			}

	
	|expr PLUS expr 
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
	| expr MOD expr
			{
				$$ = makeOperatorNode(MOD, INTTYPE, $1, $3);
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
	| ID '[' expr ']'	
		{
			$$ = makeArrayNode(ARR, $1,$3);
		}
	| ID '[' expr ']' '[' expr ']'
		{
			$$ = make2DArrayNode(ARR2D, $1, $3, $6);
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


