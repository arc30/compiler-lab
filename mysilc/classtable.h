#ifndef CLASSTABLE_H
#define CLASSTABLE_H

#include "symboltable.h"
#include "typetable.h"

typedef struct ClassFieldlist{
	char *Name;			//name of the field
	int Fieldindex;			//position of the field
	struct Typetable *Type;		//pointer to typetable 
	struct Classtable *Ctype;	//pointer to the class containing the field
	struct ClassFieldlist *Next;		//pointer to next fieldlist entry
}ClassFieldlist;

typedef struct ClassMemberfunclist {
 	char *Name;                      //name of the member function in the class
	struct Typetable *Type;          //pointer to typetable
	struct Paramstruct *paramlist;   //pointer to the head of the formal parameter list
	int Funcposition;                //position of the function in the class table
 	int Flabel;                      //A label for identifying the starting address of the function's code in the memory
	struct ClassMemberfunclist *Next;     //pointer to next Memberfunclist entry
}ClassMemberfunclist;


typedef struct Classtable {
 	char *Name;                           //name of the class
	struct ClassFieldlist *Memberfield;        //pointer to Fieldlist 
	struct ClassMemberfunclist *Vfuncptr;      //pointer to Memberfunclist
	struct Classtable *Parentptr;         //pointer to the parent's class table
	int Class_index;                      //position of the class in the virtual function table
	int Fieldcount;                       //count of fields
  	int Methodcount;                      //count of methods
	struct Classtable *Next;              //pointer to next class table entry
}Classtable;

void CInstall(char *name, char *parent_class_name);
struct Classtable* CLookup(char *name);
void Class_Finstall(Classtable *cptr, char* typeName, char *name);
void Class_Minstall(Classtable *cptr, char *name, Typetable *type, paramStruct *Paramlist);



#endif