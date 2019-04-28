#include <stdio.h>
#include <stdlib.h>
#include "typetable.h"
#include "y.tab.h"
#include <string.h>
#include "symboltable.h"
#include "classtable.h"

Classtable* ClassHead = NULL;
Classtable* ClassTail = NULL;

void endIfRedeclaredClass(char* name)
{
    Classtable* temp = ClassHead;
    while(temp)
    {
        if(strcmp(name, temp->Name)==0)
        {
           printf("Multiple Declaration of Class\n");
           exit(-1);
        }
        temp=temp->Next;
    }
}



Classtable* CInstall(char *name, char *parent_class_name) 
{
    static int classIndex = 0;
    

    endIfRedeclaredClass(name);

    Classtable* newEntry = (Classtable*)malloc(sizeof(Classtable));
    if(name == NULL ) 
    {
        printf("name null detected\n ERR\n"); exit(-1);
    }
    
    newEntry->Name = (char*)malloc(strlen(name)+1);
    strcpy(newEntry->Name,name);
    newEntry->Parentptr=NULL;
    if(parent_class_name!=NULL)
    {
        newEntry->Parentptr = malloc(strlen(parent_class_name)+1);
    }
    newEntry->Class_index = classIndex++;
    newEntry->Fieldcount=0;
    newEntry->Memberfield=NULL;
    newEntry->Methodcount=0;
    newEntry->Vfuncptr=NULL;
    newEntry->Next=NULL;


    if(ClassHead == NULL && ClassTail == NULL)
    {
        ClassHead=newEntry;
        ClassTail=newEntry;
    }
    else
    {
        ClassTail->Next = newEntry;
        ClassTail = newEntry;
    }

    TInstallClass(name, (void*)newEntry);
    return newEntry;
    
}

struct Classtable* CLookup(char *name)
{
    Classtable* temp = ClassHead;
    while(temp!=NULL)
    {
        if(strcmp(temp->Name, name)==0)
        {        
            return temp;
        }
        temp=temp->Next;
    }

    return NULL;       
}

void endIfRedeclaredClassField(ClassFieldlist* fields, char* name)
{ 
    ClassFieldlist* temp = fields;
    while(temp)
    {
        if(strcmp(temp->Name, name)==0)   
        {
            printf("Multiple Declaration of Field in class\n");
            exit(-1);
        }
        temp=temp->Next;
    }
}

void endIfRedeclaredMethod(ClassMemberfunclist* methods, char* name)
{
    ClassMemberfunclist* temp = methods;
    while(temp)
    {
        if(strcmp(temp->Name, name)==0)   
        {
            printf("Multiple Declaration of Methods in class\n");
            exit(-1);
        }
        temp=temp->Next;

    }
}


void Class_Finstall(Classtable *cptr, char* typeName, char *name)
{
    if(cptr==NULL || typeName==NULL || name==NULL)
    {
        printf("ERR Null detected in finstall\n"); exit(-1);
    }
    endIfRedeclaredClassField(cptr->Memberfield, name);
    ClassFieldlist* newEntry = (ClassFieldlist*)malloc(sizeof(ClassFieldlist));
    newEntry->Ctype = cptr;
    newEntry->Type = TLookup(typeName);
    newEntry->Name=malloc(strlen(name)+1);
    strcpy(newEntry->Name, name);
    newEntry->Next = NULL;
    cptr->Fieldcount++;
    if(cptr->Memberfield == NULL)
    {
        newEntry->Fieldindex = 0;
    }
    else
    {
        newEntry->Fieldindex = (cptr->Memberfield->Fieldindex)++;
    }
    ClassFieldlist* nextfield = cptr->Memberfield;
    cptr->Memberfield = newEntry;
    newEntry->Next = nextfield;
    
}

void Class_Minstall(Classtable *cptr, char *name, Typetable *type, struct paramStruct *Paramlist)
{
    if(cptr==NULL || name==NULL || type==NULL)
    {
        printf("ERR Null detected in minstall\n"); exit(-1);
    }    
    endIfRedeclaredMethod(cptr->Vfuncptr, name);
    ClassMemberfunclist* newEntry = (ClassMemberfunclist*)malloc(sizeof(ClassMemberfunclist));
    newEntry->Name=malloc(strlen(name)+1);
    strcpy(newEntry->Name, name);
    newEntry->paramlist= Paramlist;
    newEntry->Next = NULL;
    newEntry->Flabel = getNewFuncLabel();
    cptr->Methodcount++;
    newEntry->Type = type;

    if(cptr->Vfuncptr == NULL)
    {
        newEntry->Funcposition = 0;
        cptr->Vfuncptr = newEntry;
    }
    else
    {
        newEntry->Funcposition = (cptr->Vfuncptr->Funcposition) ++;
        newEntry->Next = cptr->Vfuncptr;
        cptr->Vfuncptr = newEntry;
    }
    
}

ClassMemberfunclist* Class_Mlookup (struct Classtable* Ctype, char* Name)
{
    if(Ctype==NULL || Name==NULL)
        {    printf("ERR Null detected in Mlookup\n"); exit(-1); }
    ClassMemberfunclist* temp = Ctype->Vfuncptr;
    while(temp)
    {
        if(strcmp(Name,temp->Name)==0)
            {return temp;}
        temp = temp->Next;
    }
    return NULL;
}

ClassFieldlist* Class_Flookup(struct Classtable* Ctype,char* Name)
{
    if(Ctype==NULL || Name==NULL)
        {    printf("ERR Null detected in Flookup\n"); exit(-1); }
    ClassFieldlist* temp = Ctype->Memberfield;
    while(temp)
    {
        if(strcmp(Name,temp->Name)==0)
            {return temp;}
        temp = temp->Next;
    }
    return NULL;
}