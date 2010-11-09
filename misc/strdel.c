/* strdel.c              A function in misclib.
   ------------------------------------------------------------------------
   int strdel(str,ch)
   char *str;
   int  ch;
   
   Deletes all ch from str.  Str becomes shorter by one for each ch deleted.
   Str may end up empty!
   
   Written: 07/09/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

int strdel(str,ch)
     char *str;
     int  ch;
{
  while (*str != '\0')
    {
      if (*str == ch)
        strlshift(str);
      else
        *str++;
    }
  return 1;
}
