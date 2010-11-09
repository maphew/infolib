/* strtrim.c             A function in misclib.
   ------------------------------------------------------------------------
   int strtrim(str,side,ch)
   char *str, *side;
   int  ch;
   
   Removes all ch from specified side of str.  Side can be "left"
   "right" or "both".  Only the first character of side is checked, so
   it could be just "l", "r", or "b".  Case of side does not matter.
   
   Written: 07/02/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

int strtrim(str,side,ch)
     char *str, *side;
     int  ch;
{
  char *strbase;
  int  len, numtodo, i, changed;
  
  strbase=str;
  changed=FALSE;
  
  /* Reject invalid ch argument. */
  
  if (ch == '\0')
    return FALSE;
  
  /* Reject invalid side arguments. */
  
  strupr(side);
  if (*side != 'L' && *side != 'R' && *side != 'B')
    return FALSE;
  
  /* Trim on right. */
  
  len=strlen(str);
  if ( (*side == 'R' || *side == 'B') && len > 0)
    {
      for (str=str+len-1; str>=strbase && *str==ch; str--)
        {
          *str='\0';
          changed=TRUE;
        }
      str=strbase;
    }
  
  /* Trim on left. */
  
  len=strlen(str);
  if ( (*side == 'L' || *side == 'B') && len > 0)
    {
      numtodo=0;
      while (*str == ch)
        {
          str++;
          numtodo++;
        }
      if (numtodo > 0)
        {
          changed=TRUE;
          str=strbase+numtodo;
          while (*str != '\0')
            {
              *strbase=*str;
              *str++;
              *strbase++;
            }
          *strbase='\0';
        }
    }
  return changed;
}
