#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "typetable.h"

typedef struct paramStruct
{
	char* paramName;
	struct Typetable* paramType;
	struct paramStruct* next;
}paramStruct;

typedef struct Lsymbol
{
	char *name;               //name of the variable
	struct Typetable* type;
	int binding;              //stores memory address allocated to the variable 
	struct Lsymbol *next;     //points to the next Local Symbol Table entry

}Lsymbol;


typedef struct Gsymbol 
{
	char *name;		// name of the variable
	struct Typetable* type;		// type of the variable
	int size;		// size of the type of the variable
	int colSize;	//column size required for 2d array
	int binding;	// stores the static memory address allocated to the variable
	struct paramStruct* paramlist; 	//for func arg
	int flabel;	
	struct Gsymbol *next;
}Gsymbol; 

Lsymbol* Llookup(char* name);
void Linstall(char* name, struct Typetable* type, int appendToBeg, int binding);

Gsymbol* Glookup(char* name); // Returns a pointer to the symbol table entry for the variable, returns NULL otherwise.

void Ginstall(char* name, struct Typetable* type, int size, int colSize, int flabel, struct paramStruct* paramlist ); // Creates a symbol table entry.

void GinstallVar(char* name, Typetable* type, int size, int colSize );
void GinstallFunc(char* name, Typetable* type, struct paramStruct* paramlist );

paramStruct* createParamNode(char* name, Typetable* type);
void appendParamNode(char* name, Typetable* type);
paramStruct* fetchParamHead();

void printSymbolTable();
void printLocalSymbolTable();
void resetParamHeadTail();
Lsymbol* fetchLsymbolHead();
void freeLsymbolTable();
void LinstallParameters(paramStruct* paramlist);
void LinstallVar(char* name, Typetable* type);










#endif