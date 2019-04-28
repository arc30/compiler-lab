#include "funcdef.h"

#include "symboltable.h"
#include "typetable.h"
#include "classtable.h"
extern Classtable* Cptr;
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void checkNameEquivalence(char* funcname, Typetable* returntype, struct paramStruct* fdefparamlist)
{   
    Typetable* typetemp = NULL;
    paramStruct* paramtemp = NULL;

    if(Cptr)
    {
        ClassMemberfunclist* temp = Class_Mlookup(Cptr, funcname);
        if(temp == NULL)
        {
            printf("ERR: Func in class %s not declared\n", funcname);
            exit(0);
        }
        typetemp = temp->Type;
        paramtemp = temp->paramlist;

    }
    else
    {
        Gsymbol* temp = Glookup(funcname);
        if(temp == NULL)
        {
            printf("ERR: Func %s not declared\n", funcname);
            exit(0);
        }
        typetemp = temp->type;
        paramtemp = temp->paramlist;

    }

    if(!checkType(returntype,typetemp))
    {
        printf("ERR: return type mismatch for %s\n",funcname);
        exit(0);
    }

    paramStruct *p1, *p2;
    p1=fdefparamlist;
    p2=paramtemp;

    while(p1 && p2)
    {
       // printf("\np1 : %s  p2:  %s  \n",p1->paramName, p2->paramName);
        if(strcmp(p1->paramName,p2->paramName)!=0)
        {
            printf("ERR: Args name mismatch for %s\n", funcname);
            exit(0);
        }
        if(!checkType(p1->paramType,p2->paramType))
        {
            printf("ERR: Argstype name mismatch for %s\n", funcname);
            exit(0);
        }

        p1=p1->next;
        p2=p2->next;

    }

    if(p1 !=NULL || p2 !=NULL)
    {
        printf("Error in signature for %s\n", funcname);
        exit(0);
    }

}