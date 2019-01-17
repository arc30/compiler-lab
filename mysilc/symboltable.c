#include "symboltable.h"
#include "y.tab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Gsymbol *head = NULL;
Gsymbol *tail = NULL;

int BindingAddr = 4096;


void printSymbolTable()
{
    Gsymbol*temp = head;
    while(temp!=NULL)
    {
        puts(temp->name);

        printf("%d  %d  %d\n", temp->type, temp->size, temp->binding);

        temp=temp->next;
    }
}

Gsymbol* lookup(char* name)
{
    Gsymbol* temp = head;
    while(temp!=NULL)
    {
        if(strcmp(temp->name, name)==0)
            return temp;
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
            printf("ERROR : Multiple declaration of variable\n");
            exit(1);
        }
        temp=temp->next;
    }
}

void install(char* name, int type, int size) // Creates a symbol table entry.
{
    endIfRedeclared(name);    

    if(BindingAddr>5119)
    {
        printf("ERROR: STACK OVERFLOW\n");
        exit(1);
    }

    Gsymbol* newEntry = (Gsymbol*) malloc(sizeof(Gsymbol));
    newEntry->name = (char*) malloc(strlen(name) + 1);
    
    strcpy(newEntry->name, name); 
    newEntry->type = type;
    newEntry->size = size; 
    newEntry->binding = BindingAddr++;
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