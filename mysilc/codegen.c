#include "codegen.h"
#include "y.tab.h"
#include <stdlib.h>

static int freeRegister=0;

int loopStack[20];	//for keeping track of loop begin and end : for breakstmt
int top=-1; 			//top for loop stack

void loopStackPush(int l1, int l2)
{
	if(top>=19)
	{
		printf("loopStack Limit exceeded\n abort");
		exit(1);
	}
	top++;
	loopStack[top]=l1;
	top++;
	loopStack[top]=l2;

}

void loopStackPop()
{
	if(top==-1)
	{
		return;
	}
	top-=2;
}

int isEmptyLoopStack()
{
	if(top==-1)
		return 1;
	return 0;
}

int loopStackTopBegin()			// return begin label of loop. 1 if l1
{
	return loopStack[top-1];
}

int loopStackTopEnd()
{
	return loopStack[top];
}

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

void endIfUndeclared(char* ch, Gsymbol* temp)
{
	if(temp == NULL)
	{
		printf("ERROR: Undeclared Variable %s ", ch);
		exit(1);
	}
}

//returns stackPos for variable
int getVarPos(char* ch)
{
	Gsymbol* temp = lookup(ch);
	
	endIfUndeclared(ch, temp);

	return temp->binding;
}

int getLabel()
{
	static int label = 0;
	return label++;
}

void pushAllRegisters(FILE* target_file)
{
	int i;
	for(i=0; i<freeRegister; i++)
	{
		fprintf(target_file, "PUSH R%d \n",i);
	}
}

void popAllRegisters(FILE* target_file)
{
	int i;
	for(i=freeRegister-1; i>=0; i--)
	{
		fprintf(target_file, "POP R%d \n",i);
	}

}

void codeGenBoundCheck(FILE* target_file, int indexReg, int size)
{
		int reg2 = getReg();
		int label_1 = getLabel();
		
		fprintf(target_file, "MOV R%d, R%d\n", reg2, indexReg); 

		fprintf(target_file, "LE R%d, R%d\n", reg2, size); 
		fprintf(target_file, "JNZ R%d, L%d\n", reg2, label_1); //out of if's body
		fprintf(target_file, "MOV R0, \"Exit\" \n");
 	  	fprintf(target_file, "PUSH R0 \n");
   		fprintf(target_file, "CALL 0 \n");
 
		freeReg();
		fprintf(target_file, "L%d:\n", label_1);

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
		
		int varPos = getVarPos(t->varname);	//returns binding of ID from symbol table
		int reg0 = getReg();
		fprintf(target_file, "MOV R%d, [%d]\n", reg0, varPos);
		return reg0;
	}

	else if(t->nodetype == ARR)
	{
		int reg0 = getReg();
		int reg1 = codeGen(t->right, target_file);
		int varPos = getVarPos(t->varname); //base address

		fprintf(target_file, "MOV R%d, %d\n", reg0, varPos);
		fprintf(target_file, "ADD R%d, R%d\n", reg1, reg0);	//reg1 has address

// TODO BOUNDCHECK
//		codeGenBoundCheck(target_file,reg1, t->gEntry->size);		

		fprintf(target_file, "MOV R%d, [R%d]\n", reg0, reg1);

		freeReg();
		return reg0;

	}

	else if(t->nodetype == ARR2D)
	{
		int reg0 = getReg();
		int reg1 = codeGen(t->right, target_file);
		int reg2 = codeGen(t->extraRight, target_file);
		int varPos = getVarPos(t->varname);

		fprintf(target_file, "MOV R%d, %d\n", reg0, t->gEntry->colSize );
		fprintf(target_file, "MUL R%d, R%d\n", reg0, reg1);
		fprintf(target_file, "ADD R%d, R%d\n", reg0, reg2);
	
		fprintf(target_file, "ADD R%d, %d \n",reg0, varPos);


		fprintf(target_file, "MOV R%d, [R%d]\n", reg0, reg0);

		freeReg();
		freeReg();
		return reg0;		
	}

	else if(t->nodetype == STRCONST)
	{
		int reg0 = getReg();
		fprintf(target_file, "MOV R%d, \"%s\"\n", reg0, t->varname);		
		return reg0;
	}
	
	else if(t->nodetype == READ)
	{

		char* varname = t->right->varname;
		int varPos = getVarPos(varname);	//base addr of variable
		pushAllRegisters(target_file);		

		int reg0 = getReg();
		fprintf(target_file, "MOV R%d, \"Read\"\n", reg0);		
		fprintf(target_file, "PUSH R%d \n", reg0);  	
		fprintf(target_file, "MOV R%d, -1 \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		
		if(t->right->nodetype == ID)
		{
			fprintf(target_file, "MOV R%d, %d \n",reg0, varPos);
			fprintf(target_file, "PUSH R%d \n", reg0);
		}
		else if(t->right->nodetype == ARR)
		{
			int regPos = codeGen(t->right->right, target_file);	//codegen for expr 
			fprintf(target_file, "ADD R%d, %d \n",regPos, varPos);

			fprintf(target_file, "PUSH R%d \n", regPos);
			freeReg();
		}
		else if(t->right->nodetype == ARR2D)
		{
			int reg2d_1 = codeGen(t->right->right, target_file);
			int reg2d_2 = codeGen(t->right->extraRight, target_file);
			fprintf(target_file, "MUL R%d, %d \n",reg2d_1, t->right->gEntry->colSize);
			fprintf(target_file, "ADD R%d, R%d \n",reg2d_1, reg2d_2);
			fprintf(target_file, "ADD R%d, %d \n",reg2d_1, varPos);

			fprintf(target_file, "PUSH R%d \n", reg2d_1);

			freeReg(); freeReg();
		}

		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		
		fprintf(target_file, "CALL 0 \n");

		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		
		freeReg();
		popAllRegisters(target_file);
		

		return -1;
		
	}

	else if(t->nodetype == WRITE)
	{

		pushAllRegisters(target_file);

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
		freeReg();

		popAllRegisters(target_file);
		return -1;
	}
	else if(t->nodetype == ASSGN)
	{
		char* varname = t->left->varname;
		int varPos = getVarPos(varname);
		int reg1 = codeGen(t->right, target_file);
		if(t->left->nodetype == ID)
		{
			fprintf(target_file, "MOV [%d], R%d\n ", varPos, reg1);
		}

		else if(t->left->nodetype == ARR)
		{
			//int regPos = getArrPos(t->left); TODO
			int regPos = codeGen(t->left->right, target_file);
			fprintf(target_file, "ADD R%d, %d \n",regPos, varPos);
			fprintf(target_file, "MOV [R%d], R%d\n ", regPos, reg1);
			freeReg();
		}

		else if(t->left->nodetype == ARR2D)
		{
			int reg2d_1 = codeGen(t->left->right, target_file);
			int reg2d_2 = codeGen(t->left->extraRight, target_file);
			fprintf(target_file, "MUL R%d, %d \n",reg2d_1, t->left->gEntry->colSize);
			fprintf(target_file, "ADD R%d, R%d \n",reg2d_1, reg2d_2);
			fprintf(target_file, "ADD R%d, %d \n",reg2d_1, varPos);

			fprintf(target_file, "MOV [R%d], R%d\n ", reg2d_1, reg1);
			freeReg();
			freeReg();
			
		}
		else if(t->left->nodetype == DEREF)
		{
			int reg2 = getReg();
			fprintf(target_file, "MOV R%d, [%d] \n",reg2, varPos);
			fprintf(target_file, "MOV [R%d], R%d \n", reg2, reg1 );
			freeReg();
		}

	
		freeReg();
		return -1;
	}
	
	else if(t->nodetype == CONNECTOR)
	{
		codeGen(t->left, target_file);	
		codeGen(t->right, target_file);
		return -1;
	}

	else if(t->nodetype == IF)
	{
		int label_1 = getLabel();
		int reg1 = codeGen(t->left, target_file);
		fprintf(target_file, "JZ R%d, L%d\n", reg1, label_1); //out of if's body
		freeReg();

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

		freeReg();

		codeGen(t->right, target_file);
		fprintf(target_file, "JMP L%d\n", label_2);

		fprintf(target_file, "L%d:\n", label_1);	//else body starts
		codeGen(t->extraRight, target_file);

		fprintf(target_file, "L%d:\n", label_2);	//end of else

		return -1;

	}

	else if(t->nodetype == WHILE)
	{
		int label_1=getLabel();	//begin label
		int label_2=getLabel();	//end label

		//push
		loopStackPush(label_1,label_2);

		fprintf(target_file, "L%d:\n", label_1);
		int reg1 = codeGen(t->left, target_file);	//code of guard
		fprintf(target_file, "JZ R%d, L%d\n", reg1, label_2);	//jmp to label2 if guard evaluates to false
		codeGen(t->right, target_file);
		fprintf(target_file, "JMP L%d\n", label_1);
		fprintf(target_file, "L%d:\n", label_2);

		loopStackPop();
	
		freeReg();
		return -1;
	}

	else if(t->nodetype == REPEAT)
	{
		int label_1 = getLabel();
		int label_2 = getLabel();

		loopStackPush(label_1,label_2);

		fprintf(target_file, "L%d:\n", label_1);
		codeGen(t->left, target_file);
		int reg1 = codeGen(t->right, target_file); 	//code of guard expr
		fprintf(target_file, "JNZ R%d, L%d\n", reg1, label_2);	//jmp to end if guard evaluates to true
		fprintf(target_file, "JMP L%d\n", label_1);
		fprintf(target_file, "L%d:\n", label_2);

		loopStackPop();

		freeReg();
		return -1;

	}

	else if(t->nodetype == BREAK)
	{
	
		if(!isEmptyLoopStack())	//breakstmt found inside loop
		{
			int label_1 = loopStackTopEnd();
			fprintf(target_file, "JMP L%d\n", label_1);
		}

		return -1;
	}

	else if(t->nodetype == CONTINUE)
	{
		if(!isEmptyLoopStack())	//called inside a loop
		{
			int label_1 = loopStackTopBegin();
			fprintf(target_file, "JMP L%d\n", label_1);
		}
		return -1;
	}
	else if(t->nodetype == DEREF)
	{
		int reg0 = getReg();
		int reg1 = codeGen(t->right, target_file);
		fprintf(target_file, "MOV R%d, [R%d]\n ",reg0, reg1);
		freeReg();
		
		return reg0;
	}
	else if(t->nodetype == ADDROF)
	{
		int varPos = getVarPos(t->right->varname);
		int reg0 = getReg();
		fprintf(target_file, "MOV R%d, %d\n ",reg0, varPos);
		return reg0;
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

			case MOD:	fprintf(target_file, "MOD R%d, R%d\n", reg1, reg2);
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
	fprintf(target_file, "MOV SP, %d\n", nextBindingAddr);	
	//4095 + 26 for storing variables. a is in [4095+1]

	int result = codeGen(t, target_file);
	
	if(freeRegister!=0)
	{
		printf("Register Leak detected !! \n");
	}


	printf("Generated Prog Code %d\n", result);



	fprintf(target_file, "MOV R0, \"Exit\" \n");
   	fprintf(target_file, "PUSH R0 \n");
   	fprintf(target_file, "CALL 0 \n");
 
   	
}


	
