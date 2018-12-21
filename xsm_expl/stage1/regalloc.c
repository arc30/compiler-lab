/*
void push( int n)
{
	struct node* temp;
	temp = (struct node*)malloc(sizeof(struct node));
	if(!temp)
	{
		printf("Heap overflow");
		exit(1);
	}
	
	temp->val=n;
	temp->next=regStackTop;
	regStackTop=temp;
}

void pop()
{
	if(regStackTop == NULL)
		{
			printf("Stack Underflow");
			exit(1);
		}
	struct node* temp;
	temp=regStackTop;
	temp->next=NULL;
	regStackTop = regStackTop->next;
	
	free(temp);
}
*/

static freeReg=0;

int getReg()
{
	if(lowestFreeReg > 19)
		{
			printf("Reg overflow");
			exit(1);
		}
		
	return freeReg++;
}

void freeReg()
{
	if(lowestFreeReg==0)
		return;
	freeReg--;
	regArr[lowestFreeReg]=0;
}	








