%option noyywrap

%{
	#include<stdio.h>
	#include <stdlib.h>
	char *file1;
	int singleCount=0, multiCount=0, b=0, c=0;
%}

singleComment	"//".*
multiComment	"/*".*

%%

{singleComment} {singleCount++;}
{multiComment}	{multiCount++;}


%%



int main()
{
	//file1="file1.txt";
  	yyin = fopen("inp.l","r");
  	yylex();
  	printf("No of singleComments: %d	No. of multiComments: %d", singleCount, multiCount);

  	return 1;
}
