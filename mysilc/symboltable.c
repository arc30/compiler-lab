#include "symboltable.h"
#include "syntaxtree.h"
#include "y.tab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Gsymbol *head = NULL;
Gsymbol *tail = NULL;

Lsymbol *LsymbolHead = NULL;
Lsymbol *LsymbolTail = NULL;

paramStruct* paramHead = NULL;		//for parameters
paramStruct* paramTail = NULL;

int nextBindingAddr = 4096;
int funcLabel = 0;

int nextLocalBinding = 0;
int nextParamBinding = -3;

void printSymbolTable()
{
    Gsymbol*temp = head;
    while(temp!=NULL)
    {
        puts(temp->name);
        if(temp->type)
        {
           printf("%s ", temp->type->name);
        }
        else
        {
            printf("Error in type is GSymbolTable\n"); exit(1);
        }
        
        printf("\n%d  %d \n", temp->size, temp->binding);
        printf("Func label: f%d\n",temp->flabel);
        printf("Parameters : ");
            if(temp->paramlist!=NULL)
            {
                paramStruct* tempptr = temp->paramlist;
                while(tempptr != NULL)
                {
                    if(tempptr->paramType!=NULL)
                    {
                        printf("%s ",tempptr->paramType->name); 
                    }
                    else 
                    {
                        printf("ERR TYPE");
                    }
                    printf("%s ",tempptr->paramName);
                    tempptr = tempptr->next;
                }
                printf("\n");
            }
            printf("\n\n");

        temp=temp->next;
    }
}


paramStruct* createParamNode(char* name, Typetable* type)
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

void appendParamNode(char* name, Typetable* type)
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

    return NULL;
}

void endIfRedeclared(char* name)
{
    Gsymbol* temp = head;
    while(temp)
    {
        if(strcmp(name, temp->name)==0)
        {
            printf("ERROR : Multiple declaration of global variable\n");
            exit(1);
        }
        temp=temp->next;
    }
}

void endIfRedeclaredLocally(char* name)
{
    Lsymbol* temp = LsymbolHead;
    while(temp)
    {
        if(strcmp(name, temp->name)==0)
        {
            printf("ERROR : Multiple declaration of local variable\n");
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

Lsymbol* fetchLsymbolHead()
{
    return LsymbolHead;
}

void Ginstall(char* name, Typetable* type, int size, int colSize, int flabel, struct paramStruct* paramlist) // Creates a symbol table entry.
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

void GinstallFunc(char* name, Typetable* type, paramStruct* paramlist)
{
    Ginstall(name,type,0,0,funcLabel,paramlist);
    funcLabel++;
}
void GinstallVar(char* name, Typetable* type, int size, int colSize)
{
    Ginstall(name,type,size,colSize,-1,NULL);
}


void Linstall(char* name, struct Typetable* type, int appendToBeg, int binding)
{
    endIfRedeclaredLocally(name);
    Lsymbol* newEntry = (Lsymbol*)malloc(sizeof(Lsymbol));
    newEntry->name = (char*)malloc(strlen(name)+1);
    strcpy(newEntry->name, name);
    newEntry->type = type;
    newEntry->binding = binding;

    newEntry->next = NULL;
    if(LsymbolHead == NULL)
    {
        LsymbolHead = newEntry;
        LsymbolTail = newEntry;
    }
    else if(appendToBeg == 1)
    {
        newEntry->next = LsymbolHead;
        LsymbolHead = newEntry;
    }
    else    //addToTheEnd
    {
        LsymbolTail->next = newEntry;
        LsymbolTail = newEntry;
    }
}

void LinstallVar(char* name, Typetable* type)
{
    Linstall(name, type, 0, ++nextLocalBinding);
}

void LinstallParameters(paramStruct* paramlist)
{
    if(paramlist != NULL)
    {
        
        Linstall(paramlist->paramName, paramlist->paramType,1,nextParamBinding--); 
        LinstallParameters(paramlist->next);
    }
}

void freeLsymbolTable()
{
    //free the entire space?
    LsymbolHead = NULL;
    LsymbolTail = NULL;
    nextLocalBinding = 0;
    nextParamBinding = -3;
}

Lsymbol* Llookup(char* name)
{
    Lsymbol* temp = LsymbolHead;
    while(temp!=NULL)
    {
        if(strcmp(temp->name, name)==0)
            {        
            return temp;
            }
        temp=temp->next;
    }
    return NULL;
}


void printLocalSymbolTable()
{
    Lsymbol*temp = LsymbolHead;
    while(temp!=NULL)
    {
        puts(temp->name);
        if(temp->type)
        {
            printf("%s ",temp->type->name);
        }
        else
        {
                printf("ERRor in Type in LSymbolTable\n"); exit(1);
        }
        
        printf("\n %d \n",  temp->binding);
      

        temp=temp->next;
    }
}


