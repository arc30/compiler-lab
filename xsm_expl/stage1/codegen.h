#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "exptree.h"

int codeGen(struct tnode* t, FILE* target_file);

#endif
