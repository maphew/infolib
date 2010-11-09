/* yesno.c               A function in misclib.
   ------------------------------------------------------------------------
   int yesno(prompt,idef)
   char *prompt;
   int  idef;
   
   Prompts with prompt and gets yes or no response.
   
   Whether default is yes, no or none is controled by idef.
   0 -- No default.  User must enter yes or no.
   1 -- Return acts like yes, but can enter no.
   2 -- Return acts like no, but can enter yes.
   
   Returns true if the response is yes, else false.
   
   Written: 07/13/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int yesno(prompt,idef)
     char *prompt;
     int  idef;
{
  int rsp, ch, done;
  rsp=-1;
  done=0;
  while (!(done))
    {
      printf("%s",prompt);
      if (idef == 1)
        printf("[yes]? ");
      if (idef == 2)
        printf("[no]? ");
      ch=getchar();
      if (ch != '\n')
        while (getchar() != '\n');
      if (ch >= 'a' && ch <= 'z')
        ch=ch-32;
      if (ch=='\n')
        {
          if (idef == 1)
            rsp=1;
          if (idef == 2)
            rsp=0;
        }
      else
        {
          if (ch=='Y')
            rsp=1;
          if (ch=='N')
            rsp=0;
        }
      if (rsp == 1 || rsp == 0)
        done=1;
      else
        printf("Please answer yes or no.\n");
    }
  return rsp;
}
