#include "syntaxtree.h"
#include "y.tab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symboltable.h"


struct tnode* createTree(int val, int type, char* c, int nodetype, Gsymbol* gEntry, struct tnode* l, struct tnode* r, struct tnode* elseptr )
{
	struct tnode* temp;
	temp = (struct tnode*)malloc(sizeof(struct tnode));
	temp->val = val;
	temp->type = type;

	temp->varname = NULL;
	
	if(c!=NULL)
	{
		temp->varname = (char*) malloc(strlen(c));
		strcpy(temp->varname, c);
	}

	temp->gEntry = gEntry;

	temp->nodetype = nodetype;
	temp->left=l;
	temp->right=r;
	temp->extraRight = elseptr;
}



int checkType(int expectedOperand1Type, int expectedOperand2Type, int operand1type, int operand2type)
{
	if (operand1type != expectedOperand1Type || operand2type != expectedOperand2Type)
	{
		return 0;
	}
	return 1;
}

void checkTypeIfElse(int guardType, int thenType, int elseType )
{
	if(guardType != BOOLTYPE || thenType != NOTYPE || elseType!=NOTYPE)
	{
		printf("Error: Type mismatch ifElseThen\n ");
		exit(1);
	}
	return;
	
}


struct tnode* makeConnectorNode(int nodetype, struct tnode* l, struct tnode* r)
{
	//checkType(NOTYPE,NOTYPE,-1, l->type, r->type, -1);
	return createTree(-1,NOTYPE,NULL,nodetype,NULL,l,r, NULL);
}

/*read and write nodes have only one child. 
Convention is to make LEFT child always null
*/

struct tnode* makeReadNode(int nodetype, struct tnode* lr)
{
	//TODO TYPECHECK READ
	//if(! checkType(-1,INTTYPE, -1,lr->type) )
	if(!checkType(-1, INTTYPE, NOTYPE, lr->type) && !checkType(-1, STRTYPE, NOTYPE, lr->type))
		{
			printf("Type Error: Read Node \n"); exit(1);
		}
	
	return createTree(-1,NOTYPE,NULL,nodetype,NULL,NULL,lr,NULL);
}

struct tnode* makeWriteNode(int nodetype, struct tnode* lr)
{
		//TODO 
		//HOW TO CHECK TYPE? CANT be done at static time no!!
		return createTree(-1,-1,NULL,nodetype,NULL,NULL,lr,NULL);
}

struct tnode* makeLeafNodeVar(int nodetype, char* ch)
{
	Gsymbol* temp = lookup(ch);

	//if lex encounters ID in declaration, there wont be a symbol table entry.
	//if lex encounters ID in program, and ID is undeclared, there wont be a gEntry.

	//TODO catch Undeclared var later.
	int type = NOTYPE;


	if(temp!=NULL)
	{
		type = temp->type;
	}

	return createTree(-1,type,ch,nodetype,temp,NULL,NULL,NULL);
}


struct tnode* makeLeafNodeNum(int nodetype, int n)
{
	return createTree(n,INTTYPE,NULL,nodetype,NULL,NULL,NULL,NULL);
}

struct tnode* makeLeafNodeStringConst(int nodetype,char* ch)
{
	return createTree(-1, STRTYPE, ch, nodetype,NULL, NULL, NULL, NULL);
}

tnode* makeArrayNode(int nodetype, tnode* l, tnode* r)
{
	char *name = l->varname;
	Gsymbol* temp = lookup(name);
	
	int type = NOTYPE;

	if(temp!=NULL)
	{
		type = temp->type;
		
		//typecheck id[expr], expr should be of int
		if(!checkType(INTTYPE, NOTYPE, r->type, NOTYPE))
		{
			printf("Type Error: Array Node : expr should evaluate to int\n"); exit(1);
		}
		
	}
	return createTree(-1,type,name,nodetype,temp,l,r,NULL);
}

tnode* make2DArrayNode(int nodetype, tnode* l, tnode* r1, tnode* r2)
{
	char *name = l->varname;
	Gsymbol* temp = lookup(name);
	
	int type = NOTYPE;

	if(temp!=NULL)
	{
		type = temp->type;
		
		//typecheck id[expr] [expr], both expr should be of int
		if(!checkType(INTTYPE, INTTYPE, r1->type, r2->type))
		{
			printf("Type Error: 2DArray Node : expr should evaluate to int\n"); exit(1);
		}
		
	}
	return createTree(-1,type,name,nodetype,temp,l,r1,r2);
}


struct tnode* makeAssignmentNode(int nodetype, char c, struct tnode* l, struct tnode* r)
{


	if(l->nodetype == ID || l->nodetype == ARR || l->nodetype == ARR2D || l->nodetype == DEREF)
	{
		if(l->type==NOTYPE)
		{
			printf("Undeclared variable %s\n", l->varname); exit(1);
		}
		if(l->nodetype == ID && l->gEntry->size > 1 ) // to check size is 1. to disallow <<int arr[10]; arr=expr>>
		{
			printf("Variable %s size doesnt match\n ", l->varname); exit(1);
		}
		if(!checkType(INTTYPE,INTTYPE, l->type,r->type) && !checkType(STRTYPE,STRTYPE, l->type,r->type) && !checkType(INTPTR,INTPTR, l->type,r->type) && !checkType(STRPTR,STRPTR, l->type,r->type)) 
		{
			printf("Type Error: Assignment Node:"); exit(1);
		}
	}

	else
	{
		printf("ERROR in ASSIGN Node. lvalue other than arr and id"); exit(1);
	}

	return createTree(-1,NOTYPE,NULL,nodetype,NULL,l,r,NULL);
}
	
struct tnode* makeOperatorNode(int nodetype, int type,struct tnode *l,struct tnode *r)
{
	//For Unary Op
	//left node is null
	if(nodetype == DEREF)
	{
		if(!checkType(NOTYPE, INTPTR, NOTYPE, r->type) && !checkType(NOTYPE, STRPTR, NOTYPE, r->type))
		{
			printf("Type Error: Dereference Node not of type ptr\n"); exit(1);
		}
		if(type == INTPTR)
		{
			type = INTTYPE;
		}
		else if (type == STRPTR)
		{
			type= STRTYPE;
		}
		return createTree(-1,type,r->varname,nodetype,NULL,l,r,NULL);
	}

	else if(nodetype == ADDROF)
	{
		if(!checkType(NOTYPE, INTTYPE, NOTYPE, r->type) && !checkType(NOTYPE, STRTYPE, NOTYPE, r->type))
		{
			printf("Type Error: AddrOf Node not of type int/str\n"); exit(1);
		}
		if(type == INTTYPE)
		{
			type = INTPTR;
		}
		else if (type == STRTYPE)
		{
			type= STRPTR;
		}

		return createTree(-1,type,r->varname,nodetype,NULL,l,r,NULL);
	}
   else
   {
	//For Binary Op
		if(!checkType(INTTYPE, INTTYPE, l->type, r->type))
		{
			printf("Type Error: Operator Node\n"); exit(1);
		}
		return createTree(-1,type,NULL,nodetype,NULL,l,r,NULL);
	}
}

struct tnode* makeIfThenElseNode(int nodetype,struct tnode* l, struct tnode* r, struct tnode* elseptr)
{
	checkTypeIfElse(l->type, r->type, elseptr->type);
	return createTree(-1, NOTYPE, NULL, nodetype, NULL, l, r, elseptr);	
}

struct tnode* makeIfThenNode(int nodetype, struct tnode* l, struct tnode* r)
{
	checkTypeIfElse(l->type, r->type, NOTYPE);

	return createTree(-1,NOTYPE,NULL,nodetype,NULL,l,r,NULL);
}	

struct tnode* makeWhileNode(int nodetype, struct tnode* l, struct tnode* r)
{
	if(!checkType(BOOLTYPE,NOTYPE, l->type,r->type))
	{
		printf("Type Error: While Node\n"); exit(1);
	}
	return createTree(-1, NOTYPE, NULL, nodetype, NULL, l, r, NULL);
}	

tnode* makeRepeatNode(int nodetype, tnode* l, tnode* r)	//repeat-until. left is slist, right is expr
{
	if(!checkType(NOTYPE,BOOLTYPE, l->type,r->type))
	{
		printf("Type Error: RepeatUntil Node\n"); exit(1);
	}
	return createTree(-1,NOTYPE,NULL,nodetype,NULL,l,r,NULL);
}

tnode* makeBreakNode(int nodetype)
{
	return createTree(-1,NOTYPE,NULL,nodetype,NULL,NULL,NULL,NULL);
}

tnode* makeContinueNode(int nodetype)
{
	return createTree(-1,NOTYPE,NULL,nodetype, NULL, NULL,NULL,NULL);
}

void printValue(struct tnode *t)
{
	
	switch(t->nodetype)
	{
		case CONNECTOR:
			break;
		case NUM: 
			printf("%d ", t->val);
			break;
		case READ:
			printf("READ ");
			break;
		case WRITE:
			printf("WRITE ");
			break;
		case ID:
			printf("%s ", t->varname);
			break;
		case PLUS:
			printf("+ ");
			break;
		case MINUS:
			printf("- ");
			break;
		case MUL:
			printf("* ");
			break;
		case DIV:
			printf("/ ");
			break;
		case MOD:
			printf("% ");
			break;
		case ASSGN:
			printf("= ");
			break;
		case GREATERTHAN_EQUAL:
			printf(">= ");
			break;
		case LESSTHAN_EQUAL:
			printf("<= ");
			break;
		case GREATERTHAN:
			printf("> ");
			break;
		case LESSTHAN:
			printf("< ");
			break;
		case EQUAL:
			printf("== ");
			break;
		case NOTEQUAL:
			printf("!= ");
			break;
		case WHILE:
			printf("WHILE ");
			break;
		case IFELSE:
			printf("IF THEN ELSE ");
			break;
		case IF:
			printf("IF ");
			break;
		case BREAK:
			printf("BREAK ");
			break;
		case CONTINUE:
			printf("CONTINUE ");
			break;
		case REPEAT:
			printf("REPEAT UNTIL ");
			break;
		case STRCONST:
			printf("%s ", t->varname);
			break;		
		case ARR:
			printf("[] ");	
			break;
		case ARR2D:
			printf("[][] ");
			break;
		case DEREF:
			printf("* ");
			break;
		case ADDROF:
			printf("& ");
			break;
		default:
			printf("unknown nodetype, %d ",t->nodetype);
			exit(1);
			
	}	

}



void inorderForm(struct tnode* t)
{
	if(t != NULL)
	{	
		inorderForm(t->left);
		printValue(t);	
		inorderForm(t->right);
		inorderForm(t->extraRight);
		
	}	
		
}


