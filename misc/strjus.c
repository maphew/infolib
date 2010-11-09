/* strjus.c              A function in misclib.
   ------------------------------------------------------------------------
   int strjus(str,side,ch,len)
   char *str, *side;
   int  ch, len;
   
   Justifies str to the left or right and padding with ch such that final
   length is len.
   
   Side can be "left" or "right".  Only the first character of side is
   checked, so it could be just "l" or "r".  Case of side does not
   matter.
   
   User must make sure str is declared long enough to contain any added
   characters.
   
   Written: 09/23/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

int strjus(str,side,ch,len)
     char *str, *side;
     int  ch, len;
{
  int retval;
  
  retval=1;
  strupr(side);
  switch (*side)
    {
    case 'R':
      strtrim(str,"both",ch);
      if (strlen(str) > len)
        retval=0;
      else
        while (strlen(str) < len)
          strinsert(str,ch,0);
      break;
    case 'L':
      strtrim(str,"both",ch);
      if (strlen(str) > len)
        retval=0;
      else
        strpad(str,ch,len);
      break;
    default:
      retval=0;
      break;
    }
  return retval;
}
