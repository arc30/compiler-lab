#include "syntaxtree.h"
#include "y.tab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "symboltable.h"


tnode* createTreeWithArglist(int val, Typetable* type, char* c, int nodetype, tnode* argslist, Gsymbol* gEntry, Lsymbol* lEntry, struct tnode* l, struct tnode* r, struct tnode* elseptr)
{
	struct tnode* temp;
	temp = (struct tnode*)malloc(sizeof(struct tnode));
	temp->val = val;
	temp->type = type;

	temp->varname = NULL;
	
	if(c!=NULL)
	{
		temp->varname = (char*) malloc(strlen(c));
		strcpy(temp->varname, c);
	}

	temp->gEntry = gEntry;
	temp->lEntry = lEntry;

	temp->nodetype = nodetype;
	temp->argslist = argslist;

	temp->left=l;
	temp->right=r;
	temp->extraRight = elseptr;
}

struct tnode* createTreeWithLenty(int val, Typetable* type, char* c, int nodetype, Gsymbol* gEntry, Lsymbol* lEntry, struct tnode* l, struct tnode* r, struct tnode* elseptr )
{

	return createTreeWithArglist(val, type, c, nodetype, NULL, gEntry, lEntry, l, r, elseptr);
}

tnode* createTree(int val, Typetable* type, char* c, int nodetype, Gsymbol* gEntry, struct tnode* l, struct tnode* r, struct tnode* elseptr )
{
	//add lEntry if exists and if nodetype is ID 
	
	Lsymbol* lEntry = NULL;
	if(nodetype == ID)
	{
		lEntry = Llookup(c);
	}		

	return createTreeWithLenty(val, type, c, nodetype, gEntry, lEntry, l, r, elseptr );

}


int checkType(Typetable* type1, Typetable* type2)
{
	if(type1==NULL || type2==NULL)
	{
		printf("Error ! NULL passed to checkType.\n "); return 0;
	}

	if(strcmp(type1->name,type2->name) != 0)
	{
		return 0;
	}
	return 1;
}


void checkTypeIfElse(Typetable* guardType, Typetable* thenType, Typetable* elseType )
{
	if(!checkType(guardType,TLookup("bool")) || !checkType(thenType,TLookup("void")) || !checkType(elseType,TLookup("void")))
	{
		printf("Error: Type mismatch ifElseThen\n ");
		exit(1);
	}
	return;
	
}

void checkTypeFuncCall(tnode* funccall)
{
	Gsymbol* temp = Glookup(funccall->varname);

	if(temp->type == NULL || funccall->type == NULL)
	{
		printf("Error ! NULL type in checkTypeFuncCall.\n "); exit(1);		
	}
	if(strcmp(funccall->type->name, temp->type->name)) 
	{
		printf("Ret Type Error func call %s\n", funccall->varname); exit(1);
	}
	
	paramStruct* pTemp = temp->paramlist;
	tnode* fTemp = funccall;

	while(fTemp->argslist != NULL && pTemp != NULL)
	{
		if(fTemp->argslist->type != pTemp->paramType)
		{
		printf("arg Type Error func call %s\n", funccall->varname); exit(1);
		}
		fTemp = fTemp->argslist;
		pTemp= pTemp->next;
	}

	//check if num of args match
	if(pTemp!=NULL || fTemp->argslist!=NULL)
	{
		printf("arg num mismatch Error func call %s\n", funccall->varname);  exit(1);
	}

}

tnode* makeTypeNode(char* typename)
{
	Typetable* type = TLookup(typename);
	return createTree(-1,type,typename,-1,NULL,NULL,NULL,NULL);
}

tnode* makeFuncCallNode(int nodetype, char* c, tnode* arglist )
{
	Typetable* type = TLookup("void");
	Gsymbol* temp = Glookup(c);
	if(temp!=NULL)
	{
		type = temp->type;
	}
	return createTreeWithArglist(-1, type, c, nodetype, arglist, temp, NULL, NULL, NULL, NULL);
}

tnode* makeFuncdefNode(int nodetype, char* ch, Typetable* type, tnode*l, tnode* r)
{
	Lsymbol* lHead = fetchLsymbolHead();

	return createTreeWithLenty(-1,type,ch,nodetype,l->gEntry,lHead,l,r,NULL);
}

tnode* makeMainNode(int nodetype, tnode* body)
{
	Lsymbol* lHead = fetchLsymbolHead();
	Typetable* type = TLookup("int");
	return createTreeWithLenty(-1,type,NULL,nodetype,NULL,lHead,NULL,body,NULL);

}

struct tnode* makeConnectorNode(int nodetype, struct tnode* l, struct tnode* r)
{
	Typetable* type = TLookup("void");
	return createTree(-1,type,NULL,nodetype,NULL,l,r, NULL);
}

/*read and write nodes have only one child. 
Convention is to make LEFT child always null
*/

struct tnode* makeReadNode(int nodetype, struct tnode* lr)
{
	//TODO TYPECHECK READ
	//if(! checkType(-1,INTTYPE, -1,lr->type) )
	if(!checkType(TLookup("int"), lr->type) && !checkType(TLookup("str"), lr->type))
		{
			printf("\nType Error: Read Node \n"); exit(1);
		}
	
	Typetable* type = TLookup("void");
	return createTree(-1,type,NULL,nodetype,NULL,NULL,lr,NULL);
}

struct tnode* makeWriteNode(int nodetype, struct tnode* lr)
{
		//TODO 
		//HOW TO CHECK TYPE? CANT be done at static time no!!
	if(!checkType(lr->type, TLookup("int")) && !checkType(lr->type, TLookup("str")))
	{
		printf("\nType ERR: write node\n"); exit(1);
	}

	Typetable* type = TLookup("void");
	return createTree(-1,type,NULL,nodetype,NULL,NULL,lr,NULL);
}

struct tnode* makeLeafNodeVar(int nodetype, char* ch)
{
	Gsymbol* temp = Glookup(ch);
	Lsymbol* Ltemp = Llookup(ch);


	//if lex encounters ID in globdeclaration, there wont be a symbol table entry.
	//if lex encounters ID in program, and ID is undeclared, there wont be a gEntry.

	//TODO catch Undeclared var later.
	Typetable* type = TLookup("void");

	if(Ltemp != NULL)
	{
		type = Ltemp->type;
	}

	else if(temp!=NULL)
	{
		type = temp->type;
	}

	return createTreeWithLenty(-1,type,ch,nodetype,temp,Ltemp,NULL,NULL,NULL);
}


struct tnode* makeLeafNodeNum(int nodetype, int n)
{
	return createTree(n,TLookup("int"),NULL,nodetype,NULL,NULL,NULL,NULL);
}

struct tnode* makeLeafNodeStringConst(int nodetype,char* ch)
{
	return createTree(-1, TLookup("str"), ch, nodetype,NULL, NULL, NULL, NULL);
}

tnode* makeFieldDeclNode(int nodetype, tnode* l, tnode* r)
{
	return createTree(-1,TLookup("void"),l->varname,nodetype,NULL,l,r,NULL );
}

tnode* makeFieldNode(int nodetype, tnode* l, tnode* r)
{

	Typetable* ftype = l->type;
	if(ftype==NULL)
	{
		printf("ERROR in field. typename not found\n"); exit(1);
	}
	Fieldlist* field = FLookup(ftype,r->varname);
	if(field == NULL)
	{
		printf("ERROR in field. fieldname not found\n"); exit(1);
	}

		
	return createTree(-1,field->type,r->varname,nodetype,NULL,l,r,NULL);
}

tnode* makeFreeNode(int nodetype, tnode* r)
{
	//expr type should be int. an addr actually. 
	if(checkType(TLookup("int"), r->type) || checkType(TLookup("str"), r->type) || checkType(TLookup("bool"), r->type) || checkType(TLookup("void"), r->type) )
	{
		printf("Type Error: invalid dealloc"); exit(1);
	}
	return createTree(-1,TLookup("void"),r->varname,nodetype,NULL,NULL,r,NULL);
}

tnode* makeInitializeNode(int nodetype, tnode* r)
{
	if(!checkType(TLookup("int"), r->type))
	{
		printf("Type Error: initialize"); exit(1);
	}
	return createTree(-1,TLookup("void"),r->varname,nodetype,NULL,NULL,r,NULL);
}

tnode* makeAllocNode(int nodetype, tnode* r)
{
	if(checkType(TLookup("int"), r->type) || checkType(TLookup("str"), r->type) || checkType(TLookup("bool"), r->type) || checkType(TLookup("void"), r->type) )
	{
		printf("Type Error: invalid alloc"); exit(1);
	}
	
	return createTree(-1,TLookup("void"),r->varname,nodetype,NULL,NULL,r,NULL);
}

tnode* makeArrayNode(int nodetype, tnode* l, tnode* r)
{
	char *name = l->varname;
	Gsymbol* temp = Glookup(name);
	Lsymbol* Ltemp = Llookup(name);
	
	Typetable* type = TLookup("void");

	if(temp!=NULL)
	{
		type = temp->type;
		
		//typecheck id[expr], expr should be of int
		if(!checkType(TLookup("int"), r->type))
		{
			printf("Type Error: Array Node : expr should evaluate to int\n"); exit(1);
		}
		
	}
	return createTreeWithLenty(-1,type,name,nodetype,temp,Ltemp,l,r,NULL);
}


tnode* makeNullNode(int nodetype)
{
	return createTree(-1,NULL,NULL,nodetype,NULL,NULL,NULL,NULL);
}

struct tnode* makeAssignmentNode(int nodetype, char c, struct tnode* l, struct tnode* r)
{


	if(l->nodetype == ID || l->nodetype == ARR || l->nodetype == FIELD)
	{

		if( checkType(l->type, TLookup("void")))
		{
			printf("Undeclared variable %s\n", l->varname); exit(1);
		}
		if(l->nodetype == ID && l->gEntry!=NULL  ) // to check size is 1. to disallow <<int arr[10]; arr=expr>>
		{   if((l->gEntry->size)>1)
				{
					printf("Variable %s size doesnt match\n ", l->varname); exit(1);
				}
		}
		
		if(r->type != NULL)			
		{
			if(!checkType(r->type, l->type))
			{printf("Type Error: Assignment Node:"); exit(1);}
		}
	}

	else
	{
		printf("ERROR in ASSIGN Node. lvalue unknown"); exit(1);
	}

	if(r->nodetype == FUNCCALL)
	{
		//type check
		checkTypeFuncCall(r);
		
	}

	Typetable* type = TLookup("void");
	return createTree(-1,type,NULL,nodetype,NULL,l,r,NULL);
}
	
struct tnode* makeOperatorNode(int nodetype, Typetable* type,struct tnode *l,struct tnode *r)
{

//For Binary Op
	if(!(checkType(TLookup("int"), l->type) && checkType(TLookup("int"), r->type)))
	{
		if(!(r->type ==NULL))	//TODO CORRECT THIS
		{	printf("Type Error: Operator Node\n"); exit(1); }
	}
	return createTree(-1,type,NULL,nodetype,NULL,l,r,NULL);
	
}

struct tnode* makeIfThenElseNode(int nodetype,struct tnode* l, struct tnode* r, struct tnode* elseptr)
{
	checkTypeIfElse(l->type, r->type, elseptr->type);
	Typetable* type = TLookup("void");
	return createTree(-1, type, NULL, nodetype, NULL, l, r, elseptr);	
}

struct tnode* makeIfThenNode(int nodetype, struct tnode* l, struct tnode* r)
{
	Typetable* type = TLookup("void");
	checkTypeIfElse(l->type, r->type, type );

	return createTree(-1,type,NULL,nodetype,NULL,l,r,NULL);
}	

struct tnode* makeWhileNode(int nodetype, struct tnode* l, struct tnode* r)
{
	if(!checkType(TLookup("bool"),l->type) || !checkType(TLookup("void"),r->type))
	{
		printf("Type Error: While Node\n"); exit(1);
	}

	Typetable* type = TLookup("void");
	return createTree(-1, type, NULL, nodetype, NULL, l, r, NULL);
}	

tnode* makeRepeatNode(int nodetype, tnode* l, tnode* r)	//repeat-until. left is slist, right is expr
{
	if(!checkType(TLookup("bool"),r->type) || !checkType(TLookup("void"),l->type))
	{
		printf("Type Error: RepeatUntil Node\n"); exit(1);
	}
	Typetable* type = TLookup("void");
	return createTree(-1,type,NULL,nodetype,NULL,l,r,NULL);
}

tnode* makeBreakNode(int nodetype)
{
	Typetable* type = TLookup("void");
	return createTree(-1,type,NULL,nodetype,NULL,NULL,NULL,NULL);
}

tnode* makeContinueNode(int nodetype)
{
	Typetable* type = TLookup("void");
	return createTree(-1,type,NULL,nodetype, NULL, NULL,NULL,NULL);
}

tnode* makeReturnNode(int nodetype, tnode* expr, Lsymbol* lentry, Typetable* returnType)
{
	// TODO TYPECHECK RETURN
	if(!checkType(returnType, expr->type ))
	{
		printf("Type Error: Return Node\n"); exit(1);
	}
	return createTreeWithLenty(-1,expr->type, NULL, nodetype, NULL, lentry, NULL,expr, NULL);
}

void printValue(struct tnode *t)
{
	
	switch(t->nodetype)
	{
		case CONNECTOR:
			break;
		case NUM: 
			printf("%d ", t->val);
			break;
		case READ:
			printf("READ ");
			break;
		case WRITE:
			printf("WRITE ");
			break;
		case ID:
			printf("%s ", t->varname);
			break;
		case PLUS:
			printf("+ ");
			break;
		case MINUS:
			printf("- ");
			break;
		case MUL:
			printf("* ");
			break;
		case DIV:
			printf("/ ");
			break;
		case MOD:
			printf("modulo ");
			break;
		case ASSGN:
			printf("= ");
			break;
		case GREATERTHAN_EQUAL:
			printf(">= ");
			break;
		case LESSTHAN_EQUAL:
			printf("<= ");
			break;
		case GREATERTHAN:
			printf("> ");
			break;
		case LESSTHAN:
			printf("< ");
			break;
		case EQUAL:
			printf("== ");
			break;
		case NOTEQUAL:
			printf("!= ");
			break;
		case WHILE:
			printf("WHILE ");
			break;
		case IFELSE:
			printf("IF THEN ELSE ");
			break;
		case IF:
			printf("IF ");
			break;
		case BREAK:
			printf("BREAK ");
			break;
		case CONTINUE:
			printf("CONTINUE ");
			break;
		case RETURN:
			printf("RETURN ");
			break;
		case REPEAT:
			printf("REPEAT UNTIL ");
			break;
		case STRCONST:
			printf("%s ", t->varname);
			break;		
		case ARR:
			printf("[] ");	
			break;
		case FIELD:
			printf(". ");
			break;
		case NULLCONST:
			printf("null ");
			break;	
		case DEREF:
			printf("* ");
			break;
		case ADDROF:
			printf("& ");
			break;
		case FUNC:
			printf("{} ");
			break;
		case MAIN:
			printf("main {} ");
			break;
		case INITIALIZE:
			printf("initialize() ");
			break;
		case ALLOC:
			printf("alloc() ");
			break;
		case DEALLOC:
			printf("free ");
			break;	
		case FUNCCALL:
			printf("%s( ", t->varname);
			while(t->argslist != NULL)
			{
				inorderForm(t->argslist);
				if(t->argslist->argslist !=NULL)
				{
					printf(",");
				}
				t=t->argslist;
			}
			printf(") ");
			break;
		default:
			printf("unknown nodetype, %d ",t->nodetype);
			exit(1);
			
	}	

}



void inorderForm(struct tnode* t)
{
	if(t != NULL)
	{	
		inorderForm(t->left);
		printValue(t);	
		inorderForm(t->right);
		inorderForm(t->extraRight);
		
	}	
		
}


