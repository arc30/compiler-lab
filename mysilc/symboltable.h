#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

typedef struct paramStruct
{
	char* paramName;
	int paramType;
	struct paramStruct* next;
}paramStruct;


typedef struct Gsymbol 
{
	char *name;		// name of the variable
	int type;		// type of the variable
	int size;		// size of the type of the variable
	int colSize;	//column size required for 2d array
	int binding;	// stores the static memory address allocated to the variable
	struct paramStruct* paramlist; 	//for func arg
	int flabel;	
	struct Gsymbol *next;
}Gsymbol; 



Gsymbol* lookup(char* name); // Returns a pointer to the symbol table entry for the variable, returns NULL otherwise.

void Ginstall(char* name, int type, int size, int colSize, int flabel, struct paramStruct* paramlist ); // Creates a symbol table entry.

void GinstallVar(char* name, int type, int size, int colSize );
void GinstallFunc(char* name, int type, struct paramStruct* paramlist );

paramStruct* createParamNode(char* name, int type);
void appendParamNode(char* name, int type);



#endif