#ifndef FUNCDEF_H
#define FUNCDEF_H

#include "syntaxtree.h"
#include "symboltable.h"
#include "typetable.h"


void checkNameEquivalence(char* funcname, Typetable* returntype, struct paramStruct* fdefparamlist);


#endif