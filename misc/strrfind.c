/* strrfind.c            A function in misclib.
   ------------------------------------------------------------------------
   int strrfind(list,item,start)
   char *list,*item;
   int  start;
   
   Returns position of the last occurrence of item in list at or
   before start.  If start is -1, or greater than the length of list,
   then start is the end.  Returns -1 if item not found at or before
   start.
   
   Written: 07/13/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int strrfind(list,item,start)
     char *list,*item;
     int  start;
{
  char *listbase, *itembase;
  int  i, j, listlen, itemlen;
  
  listlen=strlen(list);
  itemlen=strlen(item);
  if (listlen>0 && itemlen>0)
    {
      i=listlen-1;
      if (start < 0 || start > i)
        start=i;
      if (itemlen-1<=start)
        {
          listbase=list;
          itembase=item;
          for (i=start; i>=0; i--)
            {
              list=listbase+i;
              item=itembase+itemlen-1;
              for (j=itemlen-1; j>=0 && *item == *list; j--)
                {
                  if (j==0)
                    return i-itemlen+1;
                  *list--;
                  *item--;
                }
            }
        }
    }
  return -1;
}
