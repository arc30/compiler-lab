#include "syntaxtree.h"
#include "y.tab.h"
#include <stdlib.h>
#include <stdio.h>


struct tnode* createTree(int val, int type, char c, int nodetype, struct tnode* l, struct tnode* r )
{
	struct tnode* temp;
	temp = (struct tnode*)malloc(sizeof(struct tnode));
	temp->val = val;
	temp->type = type;
	//temp->varname = malloc(sizeof(char));
	temp->varname = c;
	temp->nodetype = nodetype;
	temp->left=l;
	temp->right=r;
}

struct tnode* makeConnectorNode(int nodetype, struct tnode* l, struct tnode* r)
{
	return createTree(-1,-1,-1,nodetype,l,r);
}

/*read and write nodes have only one child. 
Convention is to make LEFT child always null
*/

struct tnode* makeReadNode(int nodetype, struct tnode* lr)
{
	return createTree(-1,-1,-1,nodetype,NULL,lr);
}

struct tnode* makeWriteNode(int nodetype, struct tnode* lr)
{
		return createTree(-1,-1,-1,nodetype,NULL,lr);
}

struct tnode* makeLeafNodeVar(int nodetype, char ch)
{
	return createTree(-1,-1,ch,nodetype,NULL,NULL);
}


struct tnode* makeLeafNodeNum(int nodetype, int n)
{
	return createTree(n,-1,-1,nodetype,NULL,NULL);
}

struct tnode* makeAssignmentNode(int nodetype, char c, struct tnode* l, struct tnode* r)
{
	return createTree(-1,-1,-1,nodetype,l,r);
}
	
struct tnode* makeOperatorNode(int nodetype, char c,struct tnode *l,struct tnode *r)
{
	return createTree(-1,-1,-1,nodetype,l,r);
}
	
	/*int evaluate(struct tnode *t){
	if(t->op == NULL){
	return t->val;
	}
	else{
	switch(*(t->op)){
	case '+' : return evaluate(t->left) + evaluate(t->right);
	break;
	case '-' : return evaluate(t->left) - evaluate(t->right);
	break;
	case '*' : return evaluate(t->left) * evaluate(t->right);
	break;
	case '/' : return evaluate(t->left) / evaluate(t->right);
	break;
	}
	}
	}
	*/
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
				printf("%c ", t->varname);
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
			default:
				printf("unknown nodetype, %d ",t->nodetype);
				exit(1);
				
		}	

	}
	

	
	void prefixForm(struct tnode* t)
	{
		if(t != NULL)
		{	
			printValue(t);
			prefixForm(t->left);
			prefixForm(t->right);
		}	
		
	}
	
	void postfixForm(struct tnode* t)
	{
		if(t != NULL)
		{	
			postfixForm(t->left);
			postfixForm(t->right);
			printValue(t);	
		}	
			
	}
	
	void inorderForm(struct tnode* t)
	{
		if(t != NULL)
		{	
			inorderForm(t->left);
			printValue(t);	
			inorderForm(t->right);
			
		}	
			
	}
	

