#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

#include "symboltable.h"
#define INTTYPE 1
#define BOOLTYPE 0
#define STRTYPE 2
 //TODO MAKE THESE TOKENS
#define NOTYPE -1
#define INTPTRTYPE 3
#define STRPTRTYPE 4


typedef struct tnode
{
	int val;
	int type;
	char* varname;
	int nodetype;
	Gsymbol* gEntry;
	struct tnode *left, *right, *extraRight;
}tnode;



/*Make a leaf tnode and set the value of val field*/
struct tnode* createTree(int val, int type, char* c, int nodetype, Gsymbol* gEntry, struct tnode* l, struct tnode* r, struct tnode* elseptr );
	
struct tnode* makeConnectorNode(int nodetype, struct tnode* l, struct tnode* r);

struct tnode* makeLeafNodeStringConst(int nodetype,char* ch);

struct tnode* makeLeafNodeVar(int nodetype, char* ch);

struct tnode* makeLeafNodeNum(int nodetype, int n);

struct tnode* makeAssignmentNode(int nodetype, char c, struct tnode* l, struct tnode* r);

struct tnode* makeOperatorNode(int nodetype, int type,struct tnode *l,struct tnode *r);

struct tnode* makeReadNode(int nodetype, struct tnode* lr);

struct tnode* makeWriteNode(int nodetype, struct tnode* lr);

struct tnode* makeIfThenElseNode(int nodetype,struct tnode* l, struct tnode* r, struct tnode* elseptr);

struct tnode* makeIfThenNode(int nodetype, struct tnode* l, struct tnode* r);

struct tnode* makeWhileNode(int nodetype, struct tnode* l, struct tnode* r);

tnode* makeRepeatNode(int nodetype, tnode* l, tnode* r);	//repeat-until. left is slist, right is expr

tnode* makeBreakNode(int nodetype);

tnode* makeContinueNode(int nodetype);

void inorderForm(struct tnode* t);





int checkType(int expectedOperand1Type, int expectedOperand2Type, int operand1type, int operand2type);

void checkTypeIfElse(int guardType, int thenType, int elseType );



#endif
