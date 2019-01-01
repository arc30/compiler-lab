#include "codegen.h"
#include "y.tab.h"
#include <stdlib.h>

int varMemory[26];

//returns stackPos for variable
int getVarPos(char ch)
{
	int index=ch-'a';
	return index;
}

int evaluate(struct tnode* t)
{
	//if leaf node ie number

	if(t->nodetype == NUM)
	{
		int res = t->val;
		return res;
	}

	else if(t->nodetype == ID)
	{
		
		int varPos = getVarPos(t->varname);	//returns 0 for a and so on
		int res = varMemory[varPos];
		return res;
	}
	
	else if(t->nodetype == READ)
	{
		char varname = t->right->varname;
		int varPos = getVarPos(varname);
		int input;
		scanf("%d", &input);
		varMemory[varPos] = input;
		return -1;
		
	}

	else if(t->nodetype == WRITE)
	{
		
		int res = evaluate(t->right);
		printf("%d\n", res);

		return -1;
	}
	else if(t->nodetype == ASSGN)
	{
		char varname = t->left->varname;
		int varPos = getVarPos(varname);
		int res = evaluate(t->right);
		varMemory[varPos] = res;

		return -1;
	}
	
	else if(t->nodetype == CONNECTOR)
	{
		evaluate(t->left);
		evaluate(t->right);

		return -1;
	}

	else 
	//if(t->nodetype == PLUS || t->nodetype == MINUS || t->nodetype == MUL || t->nodetype == DIV)
	{
		int res1 = evaluate(t->left);
		int res2 = evaluate(t->right);
		
		switch(t->nodetype)
		{
			case PLUS:	res1 += res2;
						break;
			
			case MINUS:	res1 -= res2;;
						break;
			
			case MUL:	res1 *= res2;
						break;
			
			case DIV:	res1 /= res2;
						break;
		
			default : 	printf("INVALID OPERATOR");
						exit(1);				
		}
		
		return res1;
	}

	printf("INVALID LINE IN CODEGEN\n");
	return -1;
	// HANDLE -1 CASE
	
}
		


void evaluateAst(struct tnode* t)
{
	
	int result = evaluate(t);
	printf("Evaluated AST\n");
}


	
