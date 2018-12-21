struct tnode* makeLeafNode(int n)
{
	struct tnode *temp;
	temp = (struct tnode*)malloc(sizeof(struct tnode));
	temp->isOperator = 0;
	temp->op = NULL;
	temp->val = n;
	temp->left = NULL;
	temp->right = NULL;
	return temp;
}
	
	struct tnode* makeOperatorNode(char c,struct tnode *l,struct tnode *r){
	struct tnode *temp;
	temp = (struct tnode*)malloc(sizeof(struct tnode));
	temp->op = malloc(sizeof(char));
	*(temp->op) = c;
	temp->isOperator = 1;
	temp->left = l;
	temp->right = r;
	return temp;
	}
	
	int evaluate(struct tnode *t){
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
	
	void printValue(struct tnode *t)
	{
			
		if(t->isOperator == 0)	//leaf node with num
		{
			printf("%d ", t->val);
		}	
		else if(t->isOperator==1)	//op node
		{
			printf("%c ", *(t->op));	
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
	
	

