#include "codegen.h"
#include "y.tab.h"
#include <stdlib.h>

static int freeRegister=0;
extern nextBindingAddr;
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
		
	int reg= freeRegister++;
	if(reg<0)
	{printf("GetReg ERROR \n"); exit(1);}
	return reg;
}

void freeReg()
{
	if(freeRegister==0)
		return;
	freeRegister--;
}	

void freeAllReg()
{
	freeRegister=0;
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

//TODO LLOOKUP!!!!!!!!!!!!!!!

int getVarPos(FILE* target_file, char* ch, Lsymbol* Ltemp) 
{
	int reg0 = getReg();
	if(Ltemp!=NULL)
	{
		fprintf(target_file, "MOV R%d, BP\n", reg0);
		fprintf(target_file, "ADD R%d, %d\n", reg0, Ltemp->binding);		
	}
	else
	{
		Gsymbol* Gtemp = Glookup(ch);	
		endIfUndeclared(ch, Gtemp);
		fprintf(target_file, "MOV R%d, %d\n", reg0, Gtemp->binding);

	}

	return reg0;
}

int getFieldPos(FILE* target_file, tnode* fieldnode)
{
	Typetable* ttemp= fieldnode->left->type;
	Fieldlist* ftemp = FLookup(ttemp,fieldnode->right->varname);
	int reg0 = codeGen(fieldnode->left, target_file); //val of var of userdef type ie an addr here
	int binding = ftemp->fieldIndex;

	fprintf(target_file, "ADD R%d, %d\n", reg0, binding);
	return reg0;
}

int getLabel()
{
	static int label = 0;
	return label++;
}

void pushAllRegisters(FILE* target_file)
{
	int i;
	for(i=0; i<=freeRegister-1; i++)
	{
		fprintf(target_file, "PUSH R%d \n",i);
	}
}

void popAllRegisters(FILE* target_file)
{
	int i;
	for(i=freeRegister-1 ; i>=0; i--)
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

void evalAndPushArgs(tnode* aTemp, FILE* target_file)
{
	if(aTemp != NULL)
	{	
		evalAndPushArgs(aTemp->argslist, target_file);
		int reg0 = codeGen(aTemp,target_file);
		fprintf(target_file, "PUSH R%d \n", reg0);
		freeReg();
	}

}

int getLocalVarCount(Lsymbol* lEntry, paramStruct* pHead)
{
	int lCount = 0;
	while(lEntry!=NULL)
	{
		lCount++;
		lEntry=lEntry->next;
	}
	while(pHead!=NULL)
	{
		lCount--;
		pHead=pHead->next;
	}
	return lCount;
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
		
		int reg1 = getVarPos(target_file, t->varname, t->lEntry);	//returns a reg. binding of ID from symbol table

		fprintf(target_file, "MOV R%d, [R%d]\n", reg1, reg1);
		return reg1;
	}

	else if(t->nodetype == ARR)
	{
		int reg0 = getVarPos(target_file, t->varname, t->lEntry); //base address reg

		int reg1 = codeGen(t->right, target_file);
		fprintf(target_file, "ADD R%d, R%d\n", reg1, reg0);	//reg1 has address

		fprintf(target_file, "MOV R%d, [R%d]\n", reg0, reg1);

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
		pushAllRegisters(target_file);		

		int varPos = getVarPos(target_file, varname, t->right->lEntry);	//reg : base addr of variable
		int reg0 = getReg();
		fprintf(target_file, "MOV R%d, \"Read\"\n", reg0);		
		fprintf(target_file, "PUSH R%d \n", reg0);  	
		fprintf(target_file, "MOV R%d, -1 \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		
		if(t->right->nodetype == ID)
		{
			fprintf(target_file, "MOV R%d, R%d \n",reg0, varPos);
			fprintf(target_file, "PUSH R%d \n", reg0);
		}
		else if(t->right->nodetype == ARR)
		{
			int regPos = codeGen(t->right->right, target_file);	//codegen for expr 
			fprintf(target_file, "ADD R%d, R%d \n",regPos, varPos);

			fprintf(target_file, "PUSH R%d \n", regPos);
			freeReg();
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
	else if(t->nodetype == INITIALIZE)
	{
		pushAllRegisters(target_file);


		int reg0 = getReg();
		fprintf(target_file, "MOV R%d, \"Heapset\"\n", reg0);		
		fprintf(target_file, "PUSH R%d \n", reg0);  	
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "CALL 0 \n");
		fprintf(target_file, "POP R%d \n", reg0);	//ret val

		int varPos = getVarPos(target_file, t->right->varname, t->right->lEntry);
		fprintf(target_file, "MOV [R%d], R%d\n", varPos, reg0);
		freeReg();

		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);

		freeReg();
		fprintf(target_file, "BRKP\n");
		popAllRegisters(target_file);
		return -1;
	}
	else if(t->nodetype == ALLOC)
	{
		pushAllRegisters(target_file);

		int reg0 = getReg();
		int reg1 = getReg();
		fprintf(target_file, "MOV R%d, \"Alloc\"\n", reg0);		
		fprintf(target_file, "PUSH R%d \n", reg0);  	
		fprintf(target_file, "MOV R%d, 8 \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "CALL 0 \n");
		fprintf(target_file, "POP R%d \n", reg1);

		int varPos = getVarPos(target_file, t->right->varname, t->right->lEntry);
		fprintf(target_file, "MOV [R%d], R%d\n", varPos, reg1);
		freeReg();

		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);
		fprintf(target_file, "POP R%d \n", reg0);

		freeReg();
		freeReg();
		popAllRegisters(target_file);
		fprintf(target_file, "BRKP\n");

		return -1;
	}
	else if(t->nodetype == DEALLOC)
	{
		pushAllRegisters(target_file);

		int reg0 = getReg();
		int reg1 = codeGen(t->right, target_file);
		fprintf(target_file, "MOV R%d, \"Free\"\n", reg0);		
		fprintf(target_file, "PUSH R%d \n", reg0);  	
		fprintf(target_file, "PUSH R%d \n", reg1);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "PUSH R%d \n", reg0);
		fprintf(target_file, "CALL 0 \n");
		fprintf(target_file, "POP R%d \n", reg0); 	//retval


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
		int reg1 = codeGen(t->right, target_file);
		if(t->left->nodetype == ID)
		{
			int varPos = getVarPos(target_file, varname, t->left->lEntry);		
			fprintf(target_file, "MOV [R%d], R%d\n ", varPos, reg1);
		}

		else if(t->left->nodetype == ARR)
		{
			int varPos = getVarPos(target_file, varname, t->left->lEntry);		
			int regPos = codeGen(t->left->right, target_file);
			fprintf(target_file, "ADD R%d, R%d \n",regPos, varPos);
			fprintf(target_file, "MOV [R%d], R%d\n ", regPos, reg1);
			freeReg();
		}
		else if(t->left->nodetype == FIELD)
		{
			int fieldPos = getFieldPos(target_file, t->left);
			fprintf(target_file, "MOV [R%d], R%d\n ", fieldPos, reg1);

			//mem addr ofTODOOOOO
		}
		
		
		freeReg();
		freeReg();
		return -1;
	}
	
		
	else if( t->nodetype == FIELD)
	{
		int reg0 = getFieldPos(target_file,t);
		fprintf(target_file, "MOV R%d, [R%d]\n",reg0, reg0 );
		return reg0;
	}

	else if(t->nodetype == NULLCONST)
	{
		int reg0 = getReg();
		fprintf(target_file, "MOV R%d, %d\n", reg0,-1);
		return reg0;
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


	// funCall, funcDef, returnstmt

	else if(t->nodetype == FUNCCALL)
	{
		//caller save the registers
		pushAllRegisters(target_file); //save em all. 

		int retVal = getReg();
	
		int tempReg = getReg();
		//eval and push args(from last to first)
		evalAndPushArgs(t->argslist, target_file);
		//push space for return val
		fprintf(target_file, "PUSH R%d\n", tempReg);	
		//transfer control to callee
		fprintf(target_file, "CALL f%d\n",t->gEntry->flabel);

		/* CALLEE executes and returns */

		//pop retVal
		fprintf(target_file, "POP R%d\n", retVal);
		//pop args
		tnode* aTemp = t->argslist;
		while(aTemp != NULL)
		{
			fprintf(target_file, "POP R%d\n", tempReg);	
			aTemp=aTemp->argslist;
		}
		freeReg();
//UNSAFE HACK !!!! To Avoid R3 being popped, if R3 is the reg with ret val. and init we've pushed just r0,r1,r2...
//calling a freeReg before popALLReg and calling getReg after popAllReg
		freeReg();
		//retrieve registers
		popAllRegisters(target_file);

		getReg();
		return retVal;

	}
	else if(t->nodetype == MAIN)
	{
		fprintf(target_file, "m0:\n");
//setting SP to its right pos. ie after all glob decl. needed yeah?
		fprintf(target_file, "BRKP\n");
		fprintf(target_file, "MOV SP, %d\n", nextBindingAddr-1);	


		//set BP to SP
		fprintf(target_file, "MOV BP, SP\n");

		//push space for local vars
		int localVarCount = getLocalVarCount(t->lEntry, NULL ); //TODO CREATE LSYMBOL ENTRY FOR MAIN
		fprintf(target_file, "ADD SP, %d\n", localVarCount);

		codeGen(t->right, target_file);
		return -1;

	}

	else if (t->nodetype == FUNC)	//for DEFn 
	{
		//place func label
		fprintf(target_file, "f%d:\n",t->gEntry->flabel);
		//save BP
		fprintf(target_file, "PUSH BP\n");
		//set BP to SP
		fprintf(target_file, "MOV BP, SP\n");

		//push space for local vars
		int localVarCount = getLocalVarCount(t->lEntry, fetchParamHead() );
		fprintf(target_file, "ADD SP, %d\n", localVarCount);

		codeGen(t->right, target_file);
		return -1;

	}
	
	else if (t->nodetype == RETURN)
	{
		//pop out local vars
		int localVarCount = getLocalVarCount(t->lEntry, fetchParamHead() );
		fprintf(target_file, "SUB SP, %d\n", localVarCount);

		//calc return expr
		int reg0 = codeGen(t->right, target_file);
		//place in [BP-2]
		fprintf(target_file, "SUB BP, 2\n");
		fprintf(target_file, "MOV [BP], R%d\n",reg0);
		freeReg();

		//set BP to old value of BP
		fprintf(target_file, "POP BP\n");
		fprintf(target_file, "RET\n");

		return -1;
	}
	
	// stg5
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
	fprintf(target_file, "%d\n%s\n%d\n%d\n%d\n%d\n%d\n%d\n",0,"m0",0,0,0,0,0,0); 
	

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


	
