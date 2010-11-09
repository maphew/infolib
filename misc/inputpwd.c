/* inputpwd.c            A function in misclib.
   ------------------------------------------------------------------------
   char *inputpwd(prompt,str,max)
   char *prompt, *str;
   int  max;
   
   Prompts with prompt and inputs string str without echo to screen.
   Up to max chars will be stored in str, PLUS the terminating null.
   Thus, str must be declared to max+1.  Extra chars will be ignored
   until return pressed.
   
   Example:
   #define MAXLEN 80
   char string[MAXLEN+1];
   inputpwd("Password: ",string,MAXLEN);
   
   Returns pointer to str so can be fed to functions that take strings.
   
   Example: strupr(inputpwd("Password: ",string,MAXLEN));
   
   Written: 09/03/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

char *inputpwd(prompt,str,max)
     char *prompt, *str;
     int  max;
{
  system("stty -echo");
  input(prompt,str,max);
  system("stty echo");
  putchar('\n');         /* newline was not echoed; put one now. */
  return(str);
}
