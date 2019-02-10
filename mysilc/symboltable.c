#include "symboltable.h"
#include "syntaxtree.h"
#include "y.tab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Gsymbol *head = NULL;
Gsymbol *tail = NULL;

paramStruct* paramHead = NULL;		//for parameters
paramStruct* paramTail = NULL;

int nextBindingAddr = 4096;
int funcLabel = 0;

void printSymbolTable()
{
    Gsymbol*temp = head;
    while(temp!=NULL)
    {
        puts(temp->name);
        switch(temp->type)
        {
            case INTTYPE:
                printf("INT ");
                break;
            case STRTYPE:
                printf("STR ");
                break;
            case INTPTR:
                printf("INTPTR ");
                break;
            case STRPTR:
                printf("STRPTR ");
                break;
            default: 
                printf("Unknown type %d: ERR ",temp->type );
        }
        printf("\n%d  %d \n", temp->size, temp->binding);
        printf("Func label: f%d\n",temp->flabel);
        printf("Parameters : ");
            if(temp->paramlist!=NULL)
            {
                paramStruct* tempptr = temp->paramlist;
                while(tempptr != NULL)
                {
                    if(tempptr->paramType == INTTYPE) 
                        printf("INT ");
                    else if(tempptr->paramType == STRTYPE)
                        printf("STR ");
                    else 
                        printf("ERR TYPE");

                    printf("%s ",tempptr->paramName);
                    tempptr = tempptr->next;
                }
                printf("\n");
            }
            printf("\n\n");

        temp=temp->next;
    }
}

paramStruct* createParamNode(char* name, int type)
{
   paramStruct* newEntry = (paramStruct*)malloc(sizeof(paramStruct));
   if(newEntry == NULL)
   {
       printf("Error in Param node creation"); exit(0);
   }
   newEntry->paramName = name;
   newEntry->paramType = type;
   newEntry->next = NULL;

    return newEntry;
}

void appendParamNode(char* name, int type)
{
    paramStruct* temp = createParamNode(name, type);
    if(paramHead == NULL)
    {
        paramHead = temp;
        paramTail = temp;
    }
    else
    {
        paramTail->next = temp;
        paramTail = temp;
    }
    
}

Gsymbol* Glookup(char* name)
{
    Gsymbol* temp = head;
    while(temp!=NULL)
    {
        if(strcmp(temp->name, name)==0)
            {        
            return temp;
            }
        temp=temp->next;
    }
  //  printf("ERROR : UNDECLARED VARIABLE %s", name);
  //  exit(1);
    return NULL;
}

void endIfRedeclared(char* name)
{
    Gsymbol* temp = head;
    while(temp)
    {
        if(strcmp(name, temp->name)==0)
        {
            printf("ERROR : Multiple declaration of variable\n");
            exit(1);
        }
        temp=temp->next;
    }
}

void resetParamHeadTail()
{
    paramHead = NULL;
    paramTail = NULL;
}

paramStruct* fetchParamHead()
{
    return paramHead;
}

void Ginstall(char* name, int type, int size, int colSize, int flabel, struct paramStruct* paramlist) // Creates a symbol table entry.
{
    endIfRedeclared(name);    

    if(nextBindingAddr>5119)
    {
        printf("ERROR: STACK OVERFLOW\n");
        exit(1);
    }

    Gsymbol* newEntry = (Gsymbol*) malloc(sizeof(Gsymbol));
    newEntry->name = (char*) malloc(strlen(name) + 1);
    
    strcpy(newEntry->name, name); 
    newEntry->type = type;
    newEntry->size = size; 
    newEntry->binding = nextBindingAddr;
    newEntry->colSize = colSize;

    nextBindingAddr += size;

    newEntry->flabel = flabel;
    newEntry->paramlist = paramlist;

    newEntry->next = NULL;

    if(head == NULL)
    {
        head = newEntry;
        tail = newEntry;
    }
    else
    {
        tail->next = newEntry;
        tail = newEntry;
    }

}

void GinstallFunc(char* name, int type, paramStruct* paramlist)
{
    Ginstall(name,type,0,0,funcLabel,paramlist);
    funcLabel++;
}
void GinstallVar(char* name, int type, int size, int colSize)
{
    Ginstall(name,type,size,colSize,-1,NULL);
}




