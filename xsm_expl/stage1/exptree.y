%{
	#include <stdio.h>
	#include <stdlib.h>
	
	#define YYSTYPE tnode *
	
	#include "exptree.h"
	
	#include "codegen.h"
	
	int yylex(void);
	
	%}
	
	%token NUM PLUS MINUS MUL DIV END
	%left PLUS MINUS
	%left MUL DIV
	
	%%
	
	program : expr END {
	//$$ = $2;
	printf("Answer : %d\n",evaluate($1));
	
	printf("Prefix form: " );
	prefixForm($1);
	printf("\n");
	
	printf("Postfix form: " );
	postfixForm($1);
	printf("\n");
	
	printf("Generating code into targetfile: \n");
	FILE* fptr=fopen("target_file1.xsm","w");
	codeGenXsm($1, fptr);
	fclose(fptr);
	
	exit(1);
	}
	;
	
	expr : expr PLUS expr {$$ = makeOperatorNode('+',$1,$3);}
	| expr MINUS expr {$$ = makeOperatorNode('-',$1,$3);}
	| expr MUL expr {$$ = makeOperatorNode('*',$1,$3);}
	| expr DIV expr {$$ = makeOperatorNode('/',$1,$3);}
	| '(' expr ')' {$$ = $2;}
	| NUM {$$ = $1;}
	;
	
	
	
	%%
	
	yyerror(char const *s)
	{
	printf("yyerror %s",s);
	}
	
	
	int main(void) {
	yyparse();
	return 0;
	}

