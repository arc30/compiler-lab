

%{

	#include <stdio.h>
	#include <stdlib.h>

	#include "codegen.h"

	#include "funcdef.h" 
	#include "syntaxtree.h"
	#include "typetable.h"

	#include "symboltable.h"
	

	#define YYSTYPE tnode *
	
	extern int yylex();
	extern int yyparse();
	extern FILE *yyin;
	extern FILE *ltin;
	

	Typetable* currType;
	Typetable* returnType;

	void yyerror(char const *s);

	
%}

%error-verbose
	
	%token OPERATOR NUM ID END BEG CONNECTOR READ WRITE FUNC FUNCCALL
	%token IF THEN ELSE ENDIF WHILE DO ENDWHILE IFELSE ASSGN BREAK CONTINUE RETURN
	%token REPEAT UNTIL MAIN
	%token DECL ENDDECL INT STR STRCONST
	%token ARR ARR2D DEREF 
	%token TYPE ENDTYPE FIELDDECL FIELD
	%token INITIALIZE ALLOC DEALLOC

	%token INTPTR STRPTR NULLCONST

	%left EQUAL NOTEQUAL
	%left GREATERTHAN GREATERTHAN_EQUAL LESSTHAN LESSTHAN_EQUAL
	%left PLUS MINUS
	%left MUL DIV MOD
	%right ADDROF
%%
	
	start : program
			{
			printf("\nGenerating AST of program, inorderForm is \n");	
			inorderForm($1);
			printf("\n\n");
//codegen
			char* file1="targetfile.xsm";
			printf("\n\nCalling codegen \n");
			FILE *fptr = fopen(file1,"w");
			codeGenXsm($1, fptr);
			printf("Finished CodeGen\n");

			fclose(fptr); //IMP

			ltin = fopen(file1,"r");
			ltlex();

			fclose(ltin);

//codegen call end
			}
	
	program : TypeDefBlock GDeclBlock FDefBlock MainBlock 
				{			
					$$ = makeConnectorNode(CONNECTOR,$3, $4); //the full program
				}		
			| TypeDefBlock GDeclBlock MainBlock { 
									$$ = $3;  
									printf("\n gdecl code\n");
									}
			| MainBlock	{
						$$=$1;
						printf("\ncode\n");
						}
		   ;

	TypeDefBlock : TYPE ENDTYPE	
				 | TYPE TypeDefList ENDTYPE 
											
				 ;							

	TypeDefList	: TypeDefList TypeDef
				| TypeDef
				;

	TypeDef		: ID '{' FieldDeclList '}'
				{
					TInstall($1, $3);
				}
				;

	FieldDeclList :	FieldDeclList FieldDecl { $$=makeConnectorNode(CONNECTOR,$1,$2);}
				  | FieldDecl { $$=$1;}
				  ;

	FieldDecl	: TypeName ID ';'	{ $$=makeFieldDeclNode(FIELDDECL,$1,$2);
									 }
				;

	TypeName	: INT	{
							currType = TLookup("int");
							$$=makeTypeNode("int");
						}
				| STR	{
							currType = TLookup("str");
							$$=makeTypeNode("str");
						}
				| ID	{
							currType = TLookup($1->varname);
							$$=makeTypeNode($1->varname);
						}
				;


	GDeclBlock : DECL Gdeclist ENDDECL		{printf("global declaration list\n");
											printSymbolTable();	
											}
				|	DECL ENDDECL			{printf("No global decl list\n") ; }
				;

	Gdeclist : Gdeclist Gdeclr					{}			
			|Gdeclr							{}
			;

	Gdeclr	:  TypeName varlist ';'				{}
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
	
	param : TypeName ID		{   
							appendParamNode($2->varname,$1->type);	
						}
		  ;

    FDefBlock : FDefBlock FDef{ $$=makeConnectorNode(CONNECTOR, $1, $2); }
			  | FDef		{ $$=$1; }
			  ;
	
	FDef	  : 	TypeName fname '(' paramlist ')' '{' LDeclBlock codeSection  '}'
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

	LDecl		: TypeName IDList ';'
				{
				
				}
				;		

	IDList      : IDList ',' ID {LinstallVar($3->varname, currType);}
				| ID	{LinstallVar($1->varname, currType);}
				;


	MainBlock	: INT MAIN '(' ')' '{' LDeclBlock codeSection '}'
					{
					$$ = makeMainNode(MAIN,$7 );
					
					printf("\nLsymbol Table of Main\n");
					printLocalSymbolTable();
					freeLsymbolTable();	
					
					}
				;

	codeSection : BEG Body END 
	{		

//		printf("\nGenerating AST of Main, inorderForm is \n");
//		inorderForm($2);

		$$ = $2;

	}
	|	BEG END {printf("No statements\n"); $$=NULL; }	
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
		| freestmt		{$$ = $1;}
		|initializestmt {$$=$1;}
		|allocstmt {$$=$1;}
	;

	initializestmt : ID ASSGN INITIALIZE '(' ')'';'
					{$$ = makeInitializeNode(INITIALIZE, $1);}
					;
	allocstmt : ID ASSGN	ALLOC '('')' ';'
				{
					$$ = makeAllocNode(ALLOC, $1);
				}

	freestmt : DEALLOC '(' ID ')' ';'
				{
					$$=makeFreeNode(DEALLOC,$3);
				}
			;
	
	inputstmt : READ '(' ID ')' ';'	{$$ = makeReadNode(READ, $3);}
			  | READ '(' ID '[' expr ']' ')' ';' { $$ = makeReadNode(READ, makeArrayNode(ARR,$3,$5)); }
			  ;	

	outputstmt : WRITE '(' expr ')' ';' {$$ = makeWriteNode(WRITE, $3);}
		
	assgnstmt : ID ASSGN expr ';' { $$ = makeAssignmentNode(ASSGN,'=',$1,$3); }

			  | ID '[' expr ']' ASSGN expr ';'	
			  	{ $$ = makeAssignmentNode(ASSGN,'=', makeArrayNode(ARR,$1,$3), $6);	}

			  | field ASSGN expr ';'
			  {
				  $$ = makeAssignmentNode(ASSGN, '=',$1,$3 );
			  }			
			 
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

	returnstmt : RETURN expr ';'	{ 
									//BAD CODE? HOW TO SOLVE THIS?
									Lsymbol* lhead = fetchLsymbolHead();
									$$ = makeReturnNode(RETURN, $2, lhead, returnType);	
									}
			   ;

	field : field '.' ID { $$ = makeFieldNode(FIELD, $1, $3); }
		  |	ID '.'	ID			//TODO to be extended 
		  {
			  $$ = makeFieldNode(FIELD,$1,$3);
		  }
		  ;
	expr : expr PLUS expr 
			{
				$$ = makeOperatorNode(PLUS,TLookup("int"),$1,$3);
			}
	| expr MINUS expr 
			{
				$$ = makeOperatorNode(MINUS, TLookup("int"),$1,$3);
			}
	| expr MUL expr 
			{
				$$ = makeOperatorNode(MUL,TLookup("int"),$1,$3);
			}
	| expr DIV expr 
			{
				$$ = makeOperatorNode(DIV, TLookup("int"),$1,$3);
			}
	| expr MOD expr
			{
				$$ = makeOperatorNode(MOD, TLookup("int"), $1, $3);
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
	| field { $$ = $1;}
	|NULLCONST { $$ = makeNullNode(NULLCONST);}

	| STRCONST {$$ = $1;}	

	| expr GREATERTHAN expr
		{
			$$ = makeOperatorNode(GREATERTHAN, TLookup("bool"), $1, $3);

		}

	| expr LESSTHAN expr
		{
			$$ = makeOperatorNode(LESSTHAN, TLookup("bool"), $1, $3);
		}

	| expr GREATERTHAN_EQUAL expr
		{
			$$ = makeOperatorNode(GREATERTHAN_EQUAL, TLookup("bool"), $1, $3);
		}

	| expr LESSTHAN_EQUAL expr
		{
			$$ = makeOperatorNode(LESSTHAN_EQUAL, TLookup("bool"), $1, $3);
		}

	|expr EQUAL expr
		{
			$$ = makeOperatorNode(EQUAL, TLookup("bool"), $1, $3);
		}

	|expr NOTEQUAL expr
		{
			$$ = makeOperatorNode(NOTEQUAL,TLookup("bool"), $1, $3);
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
	
void yyerror(char const *s)
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

	typeTableCreate();
	currType = TLookup("void");
	returnType = TLookup("void");
	yyparse();
	return 0;
}


