#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "syntaxtree.h"

void codeGenXsm(struct tnode* t, FILE* target_file);
void evalAndPushArgs(tnode* aTemp, FILE* target_file);


#endif
