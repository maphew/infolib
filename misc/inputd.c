/* inputd.c              A function in misclib.
   ------------------------------------------------------------------------
   double inputd(prompt)
   char   *prompt;
   
   Prompts with prompt and inputs double.
   
   Written: 07/08/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

double inputd(prompt)
     char   *prompt;
{
  char   s1[80];
  int    itype;
  double dv;
  itype=0;
  while (itype != -2 && itype != -3)
    {
      printf("%s",prompt);
      gets(s1);
      strtrim(s1,"both",' ');
      strtrim(s1,"left",'+');
      itype=strtype(s1);
      if (itype == -1)
        {
          strcat(s1,".");
          itype=-2;
        }
      if (itype != -2 && itype != -3)
        printf("Numeric value expected.  Try again.\n");
      else
        dv=atof(s1);
    }
  return dv;
}
