/* inputc.c              A function in misclib.
   ------------------------------------------------------------------------
   char inputc(prompt)
   char *prompt;
   
   Prompts with prompt and inputs char.
   This works only for chars not ints.
   
   Written: 07/08/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char inputc(prompt)
     char *prompt;
{
  char ch;
  printf("%s",prompt);
  ch=getchar();
  
  /* We have our character.  Now eat extra characters until return pressed. */
  
  while (getchar() != '\n');
  return ch;
}
