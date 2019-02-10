#include "funcdef.h"

#include "symboltable.h"


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void checkNameEquivalence(char* funcname, int returntype, struct paramStruct* fdefparamlist)
{
    Gsymbol* temp = Glookup(funcname);
    if(temp == NULL)
    {
        printf("ERR: Func %s not declared\n", funcname);
        exit(0);
    }

    if(returntype != temp->type)
    {
        printf("ERR: return type mismatch for %s\n",funcname);
        exit(0);
    }

    paramStruct *p1, *p2;
    p1=fdefparamlist;
    p2=temp->paramlist;

    while(p1 && p2)
    {
       // printf("\np1 : %s  p2:  %s  \n",p1->paramName, p2->paramName);
        if(strcmp(p1->paramName,p2->paramName)!=0)
        {
            printf("ERR: Args name mismatch for %s\n", funcname);
            exit(0);
        }
        if(p1->paramType != p2->paramType)
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