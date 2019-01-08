%{
	#include <stdio.h>

	#include <stdlib.h>
    #include <string.h>

	//int number;
	//char varname;
    static int lineNumber = 1;
    char* file1;
    int pass = 1;
    FILE *fp_target;

    typedef struct node
    {
        char name[10];
        int addr;
        struct node* next;
    }node;
    node* head = NULL;
	
%}

label "L"[0-9]+
	
%%

[\n]    {
            if(pass==1)
                lineNumber++;
            else if(pass == 2)
                fprintf(fp_target, "%c", *yytext);    
                
        }

{label}":"[\n]  {   if(pass == 1)
                    { 
                        char* string = malloc(yyleng);
                        strncpy(string, yytext, yyleng-2);
                        saveLabelAddr(string);
                        printf("a label decl found at %d  \n", lineNumber) ;
                    }
                    else if (pass == 2)
                    {
                        ;       //Ignored !
                    }
                }

{label}     {
                if(pass == 2)
                {
                    fprintf(fp_target, "%d", getAddr(yytext));
                }
            }


.   {   if(pass == 1)
            {
                ;
            }
        else if(pass == 2)
            {
            fprintf(fp_target, "%c", *yytext); 
            }
            
    }	
%%

int getAddr(char* s)
{
    node* p = head;
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
    //puts(s);
    
    node* temp = (node*)malloc(sizeof(node));
    strcpy(temp->name, s);
    temp->addr = 2056 + 2*(lineNumber - 9);
    lineNumber--;
    temp->next = NULL;

    if(!head)
    {
        head = temp;
    }
    else
    {
        temp->next = head;
        head = temp;
    }
}    
int yywrap(void) 
{
    if(pass == 1)
    {
        fp_target = fopen("targetfile_final.xsm", "w");
        pass++;
        fseek(yyin, 0, SEEK_SET);
        return 0;
    }
    else
    {
        fclose(fp_target);
        return 1;
    }
}
	
   
int main()
{
    file1="targetfile.xsm";
  	yyin = fopen("targetfile.xsm","r");
  	yylex();
    node* p = head;
    while(p!=NULL)
    {  
        printf("label %s addr %d", p->name, p->addr);
        p = p->next;
    }
    return 1;
}
