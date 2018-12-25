#ifndef EXPTREE_H
#define EXPTREE_H

typedef struct tnode
{
	int isOperator;
	int val;
	char *op;
	struct tnode *left, *right;
}tnode;



/*Make a leaf tnode and set the value of val field*/
	struct tnode* makeLeafNode(int n);
	
	/*Make a tnode with opertor, left and right branches set*/
	struct tnode* makeOperatorNode(char c,struct tnode *l,struct tnode *r);
	
	/*To evaluate an expression tree*/
	int evaluate(struct tnode *t);
	
	void prefixForm(struct tnode* t);
	
	void printValue(struct tnode* t);
	
	void postfixForm(struct tnode* t);

#endif
