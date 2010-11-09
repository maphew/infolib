/* strbefore.c           A function in misclib.
   ------------------------------------------------------------------------
   int strbefore(list,search,item)
   char *list, *search, *item;
   
   Sets item to that portion of list that lies before (to the left) of
   the last (rightmost) occurrence of search.  Returns true if was
   able to pull the substring.  If returning empty string for any
   reason, returns false.  User must make sure item is big enough to
   contain the substring.
   
   Written: 07/15/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#define TRUE 1
#define FALSE 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

int strbefore(list,search,item)
     char *list, *search, *item;
{
  int len;
  *item='\0';
  len=strrfind(list,search,-1);
  if (len > 0)
    {
      if (strsubstr(list,item,0,len))
        return TRUE;
    }
  return FALSE;
}
