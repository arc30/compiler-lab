#include "codege.h"

static int freeRegister=0;

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

void codeGenInitialize_xsmPrint(struct tnode* t, FILE* target_file)
{
	fprintf(target_file, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",0,2056,0,0,0,0,0,0); 



	int result = codeGen(t, target_file);
	
	fprintf(target_file,
			"MOV SP, 4096\nMOV [4096], R%d\n",
			 result);

	
	fprintf(target_file, "MOV R0, \"Write\"\n");		
   	fprintf(target_file, "PUSH R0 \n");  	
   	fprintf(target_file, "MOV R0, -2 \n");
   	fprintf(target_file, "PUSH R0 \n");
   	fprintf(target_file, "MOV R0, [4096] \n");
   	fprintf(target_file, "PUSH R0 \n");
   	fprintf(target_file, "PUSH R0 \n");
   	fprintf(target_file, "PUSH R0 \n");
   	fprintf(target_file, "CALL 0 \n");
   	fprintf(target_file, "POP R0 \n");
   	fprintf(target_file, "POP R1 \n");
   	fprintf(target_file, "POP R1 \n");
   	fprintf(target_file, "POP R1 \n");
   	fprintf(target_file, "POP R1 \n");
   	fprintf(target_file, "MOV R0, \"Exit\" \n");
   	fprintf(target_file, "PUSH R0 \n");
   	fprintf(target_file, "CALL 0 \n");
   	
   	
   	
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
		
	
