/* strafter.c            A function in misclib.
   ------------------------------------------------------------------------
   int strafter(list,search,item)
   char *list, *search, *item;
   
   Sets item to that portion of list that lies after (to the right) of
   the first (leftmost) occurrence of search.  Returns true if was
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

int strafter(list,search,item)
     char *list, *search, *item;
{
  int start, len;
  *item='\0';
  start=strfind(list,search,0);
  if (start >= 0)
    {
      start=start+strlen(search);
      len=strlen(list)-start;
      if (strsubstr(list,item,start,len))
        return TRUE;
    }
  return FALSE;
}
