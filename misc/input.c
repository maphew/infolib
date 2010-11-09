/* input.c               A function in misclib.
   ------------------------------------------------------------------------
   char *input(prompt,str,max)
   char *prompt, *str;
   int  max;
   
   Prompts with prompt and inputs string str.  Up to max chars will be
   stored in str, PLUS the terminating null.  Thus, str must be
   declared to max+1.  Extra chars will be echoed to screen and
   ignored until return pressed.
   
   Example:
   #define MAXLEN 80
   char string[MAXLEN+1];
   input("Enter a string: ",string,MAXLEN);
   
   Returns pointer to str so can be fed to functions that take strings.
   
   Example: strupr(input("Enter a string: ",string,MAXLEN));
   
   Written: 07/07/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *input(prompt,str,max)
     char *prompt, *str;
     int  max;
{
  int  num;
  char *strbase, ch;
  strbase=str;
  printf("%s",prompt);
  ch=getchar();
  for (num=1 ;num <= max && ch != '\n'; num++)
    {
      *str=ch;
      *str++;
      ch=getchar();
    }
  if (ch != '\n')
    while (getchar() != '\n');
  *str='\0';
  return(strbase);
}
