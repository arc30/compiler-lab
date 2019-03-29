%{
	#include <stdio.h>

	#include <stdlib.h>
    #include <string.h>


    static int lineNumber = 1;
    int pass = 1;
    FILE *fp_target;

    typedef struct node
    {
        char name[10];
        int addr;
        struct node* next;
    }node;
    node* labelHead = NULL;

    int getAddr(char* s);
    void saveLabelAddr(char* s);

	
%}

label [Lfm][0-9]+

	
%%

[\n]    {
            if(pass==1)
                lineNumber++;
            else if(pass == 2)
                fprintf(fp_target, "%c", *lttext);    
                
        }

{label}":"[\n]  {   if(pass == 1)
                    { 
                        char* string = malloc(ltleng);
                        strncpy(string, lttext, ltleng-2);
                        printf("a label decl found at %d  \n", lineNumber) ;

                        saveLabelAddr(string);

                    }
                    else if (pass == 2)
                    {
                        ;       //Ignored !
                    }
                }

{label}     {
                if(pass == 2)
                {
                    fprintf(fp_target, "%d", getAddr(lttext));
                }
            }


.   {   if(pass == 1)
            {
                ;
            }
        else if(pass == 2)
            {
            fprintf(fp_target, "%c", *lttext); 
            }
            
    }	
%%

int getAddr(char* s)
{
    node* p = labelHead;
    while(p)
    {
        if( strcmp(p->name, s) == 0)    //strings match
        {
            return p->addr;
        }

        p=p->next;
    }

    return -1;
}
	
void saveLabelAddr(char* s)
{
    
    node* temp = (node*)malloc(sizeof(node));
    strcpy(temp->name, s);
    temp->addr = 2056 + 2*(lineNumber - 9);
    temp->next = NULL;

    if(!labelHead)
    {
        labelHead = temp;
    }
    else
    {
        temp->next = labelHead;
        labelHead = temp;
    }
}    
int ltwrap(void) 
{
    if(pass == 1)
    {
        fp_target = fopen("targetfile_final.xsm", "w");
        pass++;
        fseek(ltin, 0, SEEK_SET);
        return 0;
    }
    else
    {
        fclose(fp_target);
        return 1;
    }
}
	
   

