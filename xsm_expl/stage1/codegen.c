

static freeRegister=0;

int getReg()
{
	if(freeRegister > 19)
		{
			printf("Reg overflow");
			exit(1);
		}
		
	return freeRegister++;
}

void freeReg()
{
	if(freeRegister==0)
		return;
	freeRegister--;

}	

int codeGen(struct tnode* t, FILE* target_file)
{
	//if leaf node ie number

	if(!(t->isOperator))
	{
		int reg0 = getReg();
		fprintf(target_file, "MOV R%d, %d\n", reg0, t->val);
		return reg0;
	}
	
	if(t->isOperator)
	{
		int reg1 = codeGen(t->left, target_file);
		int reg2 = codeGen(t->right, target_file);
		
		switch(*(t->op))
		{
			case '+':	fprintf(target_file, "ADD R%d, R%d\n", reg1, reg2);
			break;
			
			case '-':	fprintf(target_file, "SUB R%d, R%d\n", reg1, reg2);
			break;
			
			case '*':	fprintf(target_file, "MUL R%d, R%d\n", reg1, reg2);
			break;
			
			case '/':	fprintf(target_file, "DIV R%d, R%d\n", reg1, reg2);
			break;
			
			default : 	printf("INVALID OPERATOR");
						exit(1);				
		}
		
		freeReg();
		return reg1;
	}

	return -1;
	// HANDLE -1 CASE
	
}
		
	
