/* strfind.c             A function in misclib.
   ------------------------------------------------------------------------
   int strfind(list,item,start)
   char *list,*item;
   int  start;
   
   Returns position of the first occurrence of item in list at or after
   start.  Returns -1 if item not found.
   
   Written: 07/02/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <string.h>

int strfind(list,item,start)
     char *list,*item;
     int  start;
{
  char *listbase, *itembase;
  int  i;
  if (start < strlen(list))
    {
      listbase=list;
      itembase=item;
      i=start;
      if (i != 0)
        listbase=listbase+i;
      while (*list != '\0')
        {
          list=listbase;
          item=itembase;
          while (*item != '\0' && *list != '\0' && *item == *list)
            {
              *item++;
              *list++;
              if (*item == '\0')
                return i;
            }
          *listbase++;
          i++;
        }
    }
  return -1;
}
