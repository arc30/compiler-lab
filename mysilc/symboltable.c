#include "symboltable.h"
#include "syntaxtree.h"
#include "y.tab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Gsymbol *head = NULL;
Gsymbol *tail = NULL;

extern int nextBindingAddr = 4096;


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
        printf("%d  %d  %d\n", temp->size, temp->binding, temp->colSize);

        temp=temp->next;
    }
}

Gsymbol* lookup(char* name)
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

void install(char* name, int type, int size, int colSize) // Creates a symbol table entry.
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