/* strlist.c             A function in misclib.
   ------------------------------------------------------------------------
   int strlist(list,numperline,dlm)
   char *list;
   int  numperline, dlm;
   
   Does a formatted listing of the items in list to the screen.
   Items are listed numperline per line with tabs inbetween.
   Maximum item length is 255 characters.
   Uses fmod, thus requires math.h.
   
   Written: 09/02/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <rbdpaths.h>
#include MISC_H

#define MAXITEMLEN 255

int strlist(list,numperline,dlm)
     char    *list;
     int     numperline, dlm;
{
  char    item[MAXITEMLEN+1];
  int     itemnum, numitems, itemlen;
  double  ditemnum, dnumperline;
  
  dnumperline=numperline;
  numitems=stritems(list,dlm);
  
  for (itemnum=1; itemnum<=numitems; itemnum++)
    {
      strext(list,itemnum,item,dlm);
      itemlen=strlen(item);
      ditemnum=itemnum;
      
      printf("%s",item);
      
      if (fmod(ditemnum,dnumperline) == 0)
        putchar('\n');
      else
        {
          putchar('\t');
          if (itemlen < 8)
            putchar('\t');
          if (itemnum == numitems)
            putchar('\n');
        }
    }
  return 1;
}
