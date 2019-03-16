#include <stdio.h>
#include <stdlib.h>
#include "typetable.h"
#include "syntaxtree.h"
#include "y.tab.h"
#include <string.h>
#include "symboltable.h"

Typetable* TypeHead = NULL;
Typetable* TypeTail = NULL;

Typetable* populateDefaultTypeEntry(char* name )
{

    struct Typetable* newEntry = (struct Typetable*)malloc(sizeof(Typetable)); 
    newEntry->name = malloc(sizeof(name));  
    strcpy(newEntry->name, name);
    newEntry->fields=NULL;
    newEntry->size=1;
    newEntry->next=NULL;
}

void typeTableCreate()
{
    if(TypeHead || TypeTail)
    {
        printf("Unexpected Err TypeTablePtrs\n"); exit(1);
    }
    Typetable* newEntry = populateDefaultTypeEntry("int");  //POSSIBLE ERROR?
    
    if(TypeHead == NULL && TypeTail==NULL)
    {
        //set head and tail
        TypeHead= newEntry;
        TypeTail=newEntry;
    }
    else 
    {
        printf("Typetableptr error\n"); exit(1);
    }

    Typetable* newEntry1 = populateDefaultTypeEntry("str");
    newEntry->next = newEntry1;

    newEntry = populateDefaultTypeEntry("boolean");
    newEntry1->next=newEntry;

    newEntry1 = populateDefaultTypeEntry("void");
    newEntry->next = newEntry1;

    TypeTail = newEntry1;

}

void endIfRedeclaredType(char* name)
{
    Typetable* temp = TypeHead;
    while(temp)
    {
        if(strcmp(name, temp->name)==0)
        {
           printf("Multiple Declaration of Type\n");
           exit(1);
        }
        temp=temp->next;
    }
}


Typetable* TLookup(char* name)
{
    Typetable* temp = TypeHead;
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


/*
void TInstall(tnode* idnode, tnode* fieldtree)
{
    endIfRedeclaredType( idnode->varname );

    Typetable* newEntry = (Typetable*)malloc(sizeof(Typetable));
    newEntry->name = malloc(sizeof(idnode->varname));
    strcpy(newEntry->name, idnode->varname);


    while(fieldtree !=NULL)
    {
        if(fieldtree->nodetype == FIELDDECL)
        {
            //make a fieldlist
        }
    }

    newEntry->size = size;
    newEntry->fields = Fields;
    newEntry->next = NULL;

    if(TypeHead == NULL || TypeTail==NULL )
    {
        printf("Error in setting up typetable\n"); exit(1);
    }

    TypeTail->next = newEntry;
    TypeTail = newEntry;

    //link field type, update field index...

}
*/