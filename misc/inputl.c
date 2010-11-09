/* inputl.c              A function in misclib.
   ------------------------------------------------------------------------
   long inputl(prompt)
   char *prompt;
   
   Prompts with prompt and inputs long.
   
   Written: 07/08/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

long inputl(prompt)
     char *prompt;
{
  char   s1[80], s2[80];
  int    itype;
  long   i;
  itype=0;
  while (itype != -1)
    {
      printf("%s",prompt);
      gets(s1);
      strtrim(s1,"left",'+');
      strtrim(s1,"both",' ');
      if (strlen(s1) > 0)
        {
          strtrim(s1,"left",'0');
          if (strlen(s1) == 0)
            strcpy(s1,"0");
        }
      itype=strtype(s1);
      if (itype != -1)
        printf("Integer value expected.  Try again.\n");
      else
        {
          i=atol(s1);
          sprintf(s2,"%d",i);
          if (strcmp(s1,s2) != 0)
            {
              printf("Value too large or too small.  Try again.\n");
              itype=0;
            }
        }
    }
  return i;
}
