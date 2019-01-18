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
	temp->elseptr = elseptr;
}

int getType(tnode* t)
{
	//if an identifier, get type from symbol table
	//else get type from tnode structure


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
	return createTree(-1,NOTYPE,NULL,nodetype,NULL,NULL,lr,NULL);
}

struct tnode* makeWriteNode(int nodetype, struct tnode* lr)
{
		//TODO 
		//HOW TO CHECK TYPE? BOOL EXPR AND INT EXPR ACCEPTED 
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

struct tnode* makeAssignmentNode(int nodetype, char c, struct tnode* l, struct tnode* r)
{
	if(!checkType(INTTYPE,INTTYPE, l->type,r->type) && !checkType(STRTYPE,STRTYPE, l->type,r->type)) 
	{
		printf("Type Error: Assignment Node\n"); exit(1);
	}

	return createTree(-1,NOTYPE,NULL,nodetype,NULL,l,r,NULL);
}
	
struct tnode* makeOperatorNode(int nodetype, int type,struct tnode *l,struct tnode *r)
{
	if(!checkType(INTTYPE, INTTYPE, l->type, r->type))
	{
		printf("Type Error: Operator Node\n"); exit(1);
	}
	return createTree(-1,type,NULL,nodetype,NULL,l,r,NULL);
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
		inorderForm(t->elseptr);
		
	}	
		
}


