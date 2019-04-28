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

    newEntry = populateDefaultTypeEntry("bool");
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


void endIfRedeclaredField(char* name, Fieldlist* fieldhead)
{
    Fieldlist* temp = fieldhead;
    while(temp)
    {
        if(strcmp(name, temp->name)==0)
        {
           printf("Multiple Declaration of Field\n");
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

//remove the global

Fieldlist* insertInFieldList(Typetable* newTypeEntry,Fieldlist* fieldhead, char* typename,char* fieldname, tnode* typenode, int fieldindex)
{
    endIfRedeclaredField(fieldname,fieldhead);
    Fieldlist* newEntry = (Fieldlist*) malloc(sizeof(Fieldlist));
    newEntry->name = malloc(sizeof(fieldname));
    strcpy(newEntry->name, fieldname);
    newEntry->next = NULL;
    
    if(typenode->type == NULL)  //either error OR recursive type
    {
        //if recursive type
        if(strcmp(typenode->varname,typename)==0)
        {
            newEntry->type = newTypeEntry;
        }
        else
        {
            printf("Type Error: %s field \n", typename); 
            exit(1);
        } 
    }
    else
    {
        newEntry->type = typenode->type;
    }
    newEntry->fieldIndex = fieldindex;

    if(fieldhead == NULL)
    {
        fieldhead = newEntry;
        
    }
    else
    {
        newEntry->next = fieldhead;
        fieldhead = newEntry;
    }

    return fieldhead;
}

void TInstallMain(void* idnod, void* fieldtre, void* classEntry)
{
    tnode* idnode = (tnode*) idnod;
    tnode* fieldtree = (tnode*) fieldtre;
 //hack used above. wasnt able to include tnode* in typetable.h. USed void*   
    endIfRedeclaredType(idnode->varname );

    Typetable* newEntry = (Typetable*)malloc(sizeof(Typetable));
    newEntry->name = malloc(sizeof(idnode->varname));
    strcpy(newEntry->name, idnode->varname);

    Fieldlist* FieldHead = NULL;
    int fieldindex = 0;

    tnode* currNode = fieldtree;
    while(1)
    {
        if(currNode->nodetype == FIELDDECL)
        {
            FieldHead = insertInFieldList(newEntry,FieldHead,idnode->varname,currNode->right->varname,currNode->left,fieldindex++);
            break;
        }
        else if(currNode->nodetype == CONNECTOR)
        {
            FieldHead= insertInFieldList(newEntry,FieldHead,idnode->varname,currNode->right->right->varname,currNode->right->left,fieldindex++);   
            currNode = currNode->left;
        }
    }

    newEntry->size = fieldindex;
    newEntry->fields = FieldHead;
    newEntry->next = NULL;
    newEntry->classEntry = classEntry;

    if(TypeHead == NULL || TypeTail==NULL )
    {
        printf("Error in setting up typetable\n"); exit(1);
    }

    TypeTail->next = newEntry;
    TypeTail = newEntry;
}

void TInstall(void* idnod, void* fieldtre)
{
    TInstallMain(idnod, fieldtre, NULL);
}
void TInstallClass(char* name, void* classEntry)
{
    endIfRedeclaredType(name );    
    Typetable* newEntry = (Typetable*)malloc(sizeof(Typetable));
    newEntry->name = malloc(sizeof(name));
    strcpy(newEntry->name, name);
    newEntry->size = 0;
    newEntry->fields = NULL;
    newEntry->next = NULL;
    newEntry->classEntry = classEntry;

    if(TypeHead == NULL || TypeTail==NULL )
    {
        printf("Error in setting up typetable\n"); exit(-1);
    }

    TypeTail->next = newEntry;
    TypeTail = newEntry;    


}

Fieldlist* FLookup(Typetable* type, char* name)
{
    Fieldlist* ftemp = type->fields;
    while(ftemp!=NULL)
    {
        if(strcmp(ftemp->name, name)==0)
        {
            return ftemp;
        }
        ftemp=ftemp->next;
    }
    return NULL;
}
