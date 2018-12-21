#include <stdlib.h>

typedef struct node
{	
	int index;
	struct node* next;
}node;

struct node* regStackTop;
int lowestFreeReg;
int regArr[20];
