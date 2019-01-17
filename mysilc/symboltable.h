#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H


typedef struct Gsymbol 
{
	char *name;		// name of the variable
	int type;		// type of the variable
	int size;		// size of the type of the variable
	int binding;	// stores the static memory address allocated to the variable
	struct Gsymbol *next;
}Gsymbol; 



Gsymbol* lookup(char* name); // Returns a pointer to the symbol table entry for the variable, returns NULL otherwise.

void install(char* name, int type, int size); // Creates a symbol table entry.





#endif