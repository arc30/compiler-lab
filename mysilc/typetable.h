#ifndef TYPETABLE_H
#define TYPETABLE_H


typedef struct Typetable
{
    char *name;                 //type name
    int size;                   //size of the type
    struct Fieldlist *fields;   //pointer to the head of fields list
    struct Typetable *next;     // pointer to the next type table entry
    void* classEntry;       //hack used to avoid including classtable.h (circular err)
                            //pointer to class table entry in case this tyoe is of a class
                            //otherwise set null
} Typetable;


typedef struct Fieldlist
{
  char *name;              //name of the field
  int fieldIndex;          //the position of the field in the field list
  struct Typetable *type;  //pointer to type table entry of the field's type
  struct Fieldlist *next;  //pointer to the next field
}Fieldlist;


Typetable* TLookup(char* name);
void TInstall(void* idnod, void* fieldtre);
void typeTableCreate();
Fieldlist* FLookup(Typetable* type, char* name);
void TInstallClass(char* name, void* classEntry);
void TInstallMain(void* idnod, void* fieldtre, void* classEntry);





#endif