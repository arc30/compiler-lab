#ifndef SYNTAXTREE_H
#define SYNTAXTREE_H



typedef struct tnode
{
	int val;
	int type;
	char varname;
	int nodetype;
	struct tnode *left, *right;
}tnode;



/*Make a leaf tnode and set the value of val field*/
struct tnode* createTree(int val, int type, char c, int nodetype, struct tnode* l, struct tnode* r );
	
struct tnode* makeConnectorNode(int nodeid, struct tnode* l, struct tnode* r);

struct tnode* makeLeafNodeVar(int nodeid, char ch);

struct tnode* makeLeafNodeNum(int nodeid, int n);

struct tnode* makeAssignmentNode(int nodeid, char c, struct tnode* l, struct tnode* r);

struct tnode* makeOperatorNode(int nodeid, char c,struct tnode *l,struct tnode *r);

struct tnode* makeReadNode(int nodeid, struct tnode* lr);

struct tnode* makeWriteNode(int nodeid, struct tnode* lr);




#endif
