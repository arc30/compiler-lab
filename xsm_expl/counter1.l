%{

/* Scan and return a token for identifiers of the format :
             string number
Note: strings are not case sensitive
Examples: a0 , A1 , ab2 , AB4 , aBc5
*/

#include<stdio.h>

#define ID 1  //Identifier token
#define ER 2  //Error token
int lines=0, words=0,s_letters=0,c_letters=0, num=0, spl_char=0,total=0;

%}

low    [a-z]
upp    [A-Z]
number [0-9]




%option noyywrap

%%

[a-z] s_letters++;
[A-Z] c_letters++;
[0-9] num++;
\n	{lines++; words++;}
[\t ' '] {words++;}

%%

int main()
{
  
yyin= fopen("myfile.txt","r");
yylex();
total=s_letters+c_letters+num+spl_char;
printf(" This File contains ...");
printf("\n\t%d lines", lines);
printf("\n\t%d words",words);
printf("\n\t%d small letters", s_letters);
printf("\n\t%d capital letters",c_letters);
printf("\n\t%d digits", num);
printf("\n\t%d special characters",spl_char);
printf("\n\tIn total %d characters.\n",total);
}
