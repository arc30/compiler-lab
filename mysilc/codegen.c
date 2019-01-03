#include "codegen.h"
#include "y.tab.h"
#include <stdlib.h>

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

void freeAllReg()
{
	freeRegister = 0;
}

//returns stackPos for variable
int getVarPos(char ch)
{
	int index=ch-'a';
	return 4096+index;
}

int codeGen(struct tnode* t, FILE* target_file)
{
	//if leaf node ie number

	if(t->nodetype == NUM)
	{
		int reg0 = getReg();
		fprintf(target_file, "MOV R%d, %d\n", reg0, t->val);
		return reg0;
	}

	else if(t->nodetype == ID)
	{
		
		int varPos = getVarPos(t->varname);	//returns 4096+0 for a and so on
		int reg0 = getReg();
		fprintf(target_file, "MOV R%d, [%d]\n", reg0, varPos);
		return reg0;
	}
	
	else if(t->nodetype == READ)
	{
		char varname = t->right->varname;
		int varPos = getVarPos(varname);
		fprintf(target_file, "MOV R0, \"Read\"\n");		
		fprintf(target_file, "PUSH R0 \n");  	
		fprintf(target_file, "MOV R0, -1 \n");
		fprintf(target_file, "PUSH R0 \n");
		fprintf(target_file, "MOV R0, %d \n", varPos);
		fprintf(target_file, "PUSH R0 \n");
		fprintf(target_file, "PUSH R0 \n");
		fprintf(target_file, "PUSH R0 \n");
		fprintf(target_file, "CALL 0 \n");
		fprintf(target_file, "POP R0 \n");
		fprintf(target_file, "POP R1 \n");
		fprintf(target_file, "POP R1 \n");
		fprintf(target_file, "POP R1 \n");
		fprintf(target_file, "POP R1 \n");
		return -1;
		
	}

	else if(t->nodetype == WRITE)
	{
		fprintf(target_file, "MOV R0, \"Write\"\n");		
		fprintf(target_file, "PUSH R0 \n");  	
		fprintf(target_file, "MOV R0, -2 \n");
		fprintf(target_file, "PUSH R0 \n");

		int reg1 = codeGen(t->right, target_file);

		fprintf(target_file, "PUSH R%d \n", reg1);
		fprintf(target_file, "PUSH R0 \n");
		fprintf(target_file, "PUSH R0 \n");
		fprintf(target_file, "CALL 0 \n");
		fprintf(target_file, "POP R0 \n");
		fprintf(target_file, "POP R1 \n");
		fprintf(target_file, "POP R1 \n");
		fprintf(target_file, "POP R1 \n");
		fprintf(target_file, "POP R1 \n");
		freeReg();
		return -1;
	}
	else if(t->nodetype == ASSGN)
	{
		char varname = t->left->varname;
		int varPos = getVarPos(varname);
		int reg1 = codeGen(t->right, target_file);
		fprintf(target_file, "MOV [%d], R%d\n ", varPos, reg1);
		return -1;
	}
	
	else if(t->nodetype == CONNECTOR)
	{
		codeGen(t->left, target_file);
		freeReg();
		codeGen(t->right, target_file);
		freeReg();
		return -1;
	}

	else 
	//if(t->nodetype == PLUS || t->nodetype == MINUS || t->nodetype == MUL || t->nodetype == DIV)
	{
		int reg1 = codeGen(t->left, target_file);
		int reg2 = codeGen(t->right, target_file);
		
		switch(t->nodetype)
		{
			case PLUS:	fprintf(target_file, "ADD R%d, R%d\n", reg1, reg2);
						break;
			
			case MINUS:	fprintf(target_file, "SUB R%d, R%d\n", reg1, reg2);
						break;
			
			case MUL:	fprintf(target_file, "MUL R%d, R%d\n", reg1, reg2);
						break;
			
			case DIV:	fprintf(target_file, "DIV R%d, R%d\n", reg1, reg2);
						break;
		
			default : 	printf("INVALID OPERATOR");
						exit(1);				
		}
		
		freeReg();
		return reg1;
	}

	printf("INVALID LINE IN CODEGEN\n");
	return -1;
	// HANDLE -1 CASE
	
}
		


void codeGenXsm(struct tnode* t, FILE* target_file)
{
	fprintf(target_file, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n%d\n",0,2056,0,0,0,0,0,0); 
	fprintf(target_file, "BRKP\n");
	fprintf(target_file, "MOV SP, 4121\n");	
	//4095 + 26 for storing variables. a is in [4095+1]

	int result = codeGen(t, target_file);
	printf("Generated Prog Code %d\n", result);

	fprintf(target_file, "MOV R0, \"Exit\" \n");
   	fprintf(target_file, "PUSH R0 \n");
   	fprintf(target_file, "CALL 0 \n");
 
   	
}


	
