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



//returns stackPos for variable
int getVarPos(char ch)
{
	int index=ch-'a';
	return 4096+index;
}

int getLabel()
{
	static int label = 0;
	return label++;
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
		int reg0 = getReg();
		fprintf(target_file, "MOV R%d, \"Read\"\n", reg0);		
		fprintf(target_file, "PUSH R%d \n", reg0);  	
		fprintf(target_file, "MOV R%d, -1 \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "MOV R%d, %d \n",reg0, varPos);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "CALL 0 \n");
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		return -1;
		
	}

	else if(t->nodetype == WRITE)
	{

		int reg1 = codeGen(t->right, target_file);
		int reg0 = getReg();
		fprintf(target_file, "MOV R%d, \"Write\"\n", reg0);		
		fprintf(target_file, "PUSH R%d \n", reg0);  	
		fprintf(target_file, "MOV R%d, -2 \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg1);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "CALL 0 \n");
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
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

	else if(t->nodetype == IF)
	{
		int label_1 = getLabel();
		int reg1 = codeGen(t->left, target_file);
		fprintf(target_file, "JZ R%d, L%d\n", reg1, label_1); //out of if's body
		codeGen(t->right, target_file);
		fprintf(target_file, "L%d:\n", label_1);

		return -1;
	}

	else if(t->nodetype == IFELSE)
	{
		int label_1 = getLabel();
		int label_2 = getLabel();
		int reg1 = codeGen(t->left, target_file);

		fprintf(target_file, "JZ R%d, L%d\n", reg1, label_1); //out of if's body
		codeGen(t->right, target_file);
		fprintf(target_file, "JMP L%d\n", label_2);


		fprintf(target_file, "L%d:\n", label_1);	//else body starts
		codeGen(t->elseptr, target_file);

		fprintf(target_file, "L%d:\n", label_2);	//end of else

		return -1;

	}

	else if(t->nodetype == WHILE)
	{
		int label_1=getLabel();
		int label_2=getLabel();
		fprintf(target_file, "L%d:\n", label_1);
		int reg1 = codeGen(t->left, target_file);	//code of guard
		fprintf(target_file, "JZ R%d, L%d\n", reg1, label_2);	//jmp to label2 if guard evaluates to false
		codeGen(t->right, target_file);
		fprintf(target_file, "JMP L%d\n", label_1);
		fprintf(target_file, "L%d:\n", label_2);

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

			case GREATERTHAN_EQUAL:	
						fprintf(target_file, "GE R%d, R%d\n", reg1, reg2);
						break;

			case GREATERTHAN:	
						fprintf(target_file, "GT R%d, R%d\n", reg1, reg2);
						break;

			case LESSTHAN_EQUAL:	
						fprintf(target_file, "LE R%d, R%d\n", reg1, reg2);
						break;

			case LESSTHAN:	
						fprintf(target_file, "LT R%d, R%d\n", reg1, reg2);
						break;

			case EQUAL:	
						fprintf(target_file, "EQ R%d, R%d\n", reg1, reg2);
						break;

			case NOTEQUAL:	
						fprintf(target_file, "NE R%d, R%d\n", reg1, reg2);
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


	
