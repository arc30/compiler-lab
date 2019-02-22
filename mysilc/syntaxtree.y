

%{

	#include <stdio.h>
	#include <stdlib.h>

	#include "codegen.h"

	#include "funcdef.h" 
	#include "syntaxtree.h"

	#include "symboltable.h"
	

	#define YYSTYPE tnode *
	
	extern int yylex();
	extern int yyparse();
	extern FILE *yyin;
	extern FILE *ltin;
	

	int currType = NOTYPE;
	int returnType = NOTYPE;

	
	
%}

%error-verbose
	
	%token OPERATOR NUM ID END BEG CONNECTOR READ WRITE FUNC FUNCCALL
	%token IF THEN ELSE ENDIF WHILE DO ENDWHILE IFELSE ASSGN BREAK CONTINUE RETURN
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
	
	
	
	program : GDeclBlock FDefBlock codeSection 
				{	
					printf("\nGenerating AST for FDef, inorderForm is \n");
					inorderForm($2);		
			
				}		
			| GDeclBlock codeSection {printf("\n gdecl code\n");}
			| codeSection	{printf("\ncode\n");}
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

	type	: INT						{ 
											currType = INTTYPE; 
											$$ = makeTypeNode(INTTYPE);
										}
			| STR						{   
											 currType = STRTYPE; 
											 $$ = makeTypeNode(STRTYPE);
										}
			;

	varlist : varlist ',' var				
			| var					
			;


	fname : ID 			{	
							$$ = $1; 
							returnType = currType;
						}	
	
	var : 		ID						{
										 GinstallVar($1->varname, currType, 1, 0);
										}
			| ID '[' NUM ']'			{
										 GinstallVar($1->varname, currType, $3->val, 0);
										}
			| ID '[' NUM ']' '[' NUM ']'{
										GinstallVar($1->varname, currType, ($3->val)*($6->val), $6->val);
										}

			| fname '(' paramlist ')'		{
											//Ginstall function in gsymboltable
											
											
											GinstallFunc($1->varname, returnType, fetchParamHead());
											resetParamHeadTail();
											
											
										}

			

			;

	paramlist :%empty 
			  |paramlist ',' param
			  | param	{}
			  ;
	
	param : type ID		{   
							appendParamNode($2->varname,$1->type);	
						}
		  ;

    FDefBlock : FDefBlock FDef{ $$=makeConnectorNode(CONNECTOR, $1, $2); }
			  | FDef		{ $$=$1; }
			  ;
	
	FDef	  : type ID '(' paramlist ')' '{' LDeclBlock Body '}'
				{
					checkNameEquivalence($2->varname, $1->type, fetchParamHead() );

					$$ = makeFuncdefNode(FUNC,$2->varname,$1->type,$2,$8);

					printf("\nLsymbol Table of %s\n", $2->varname);
					printLocalSymbolTable();


					freeLsymbolTable();
					resetParamHeadTail();


				}
			  ;

	LDeclBlock : DECL LDeclList ENDDECL { 
										LinstallParameters(fetchParamHead()); 	
										//Linstall in the begng
										}
			   | DECL ENDDECL { LinstallParameters(fetchParamHead()); }
			   ;

	LDeclList : LDeclList LDecl 
			   | LDecl
			   ;

	LDecl		: type IDList ';'
				{
				
				}
				;		

	IDList      : IDList ',' ID {LinstallVar($3->varname, currType);}
				| ID	{LinstallVar($1->varname, currType);}
				;



	codeSection : BEG Body END 
	{		

		printf("\nGenerating AST, inorderForm is \n");
		inorderForm($2);


		char* file1="targetfile.xsm";
		printf("\n\nCalling codegen \n");
		FILE *fptr = fopen(file1,"w");
		codeGenXsm($2, fptr);
		printf("Finished CodeGen\n");

		fclose(fptr); //IMP

		ltin = fopen(file1,"r");
		ltlex();

		fclose(ltin);
		


	}
	|	BEG END {printf("No statements\n");  }	
	;




	Body  : slist	{$$=$1;}
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
		| returnstmt	{$$ = $1;}
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

	returnstmt : RETURN expr ';'	{ $$ = makeReturnNode(RETURN, $2);	}
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
	| ID '(' ')'	{ $$ = makeFuncCallNode(FUNCCALL, $1->varname, NULL); }
	| ID '(' exprlist ')'	{ $$ = makeFuncCallNode(FUNCCALL, $$->varname, $3); }
	;

	exprlist : exprlist ',' expr	{   tnode* temp = $1;
										while(temp != NULL && temp->argslist!=NULL)
										{
											temp=temp->argslist;
										}
										temp->argslist = $3;
										$$ = $1;
									}
	        | expr	{$$ = $1;}
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


