#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H

#define inttype 1
#define booltype 0
#define typeless -1

typedef struct tnode
{
	int val;
	int type;
	char varname;
	int nodetype;
	struct tnode *left, *right, *elseptr;
}tnode;



/*Make a leaf tnode and set the value of val field*/
struct tnode* createTree(int val, int type, char c, int nodetype, struct tnode* l, struct tnode* r, struct tnode* elseptr );
	
struct tnode* makeConnectorNode(int nodetype, struct tnode* l, struct tnode* r);

struct tnode* makeLeafNodeVar(int nodetype, char ch, int type);

struct tnode* makeLeafNodeNum(int nodetype, int n, int type);

struct tnode* makeAssignmentNode(int nodetype, char c, struct tnode* l, struct tnode* r);

struct tnode* makeOperatorNode(int nodetype, int type,struct tnode *l,struct tnode *r);

struct tnode* makeReadNode(int nodetype, struct tnode* lr);

struct tnode* makeWriteNode(int nodetype, struct tnode* lr);




#endif
