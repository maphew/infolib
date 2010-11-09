/* strpad.c              A function in misclib.
   ------------------------------------------------------------------------
   int strpad(string,ch,len)
   char *string;
   int  ch, len;
   
   Pads string with ch to length len.
   
   If string is already as long or longer than the len specified, then
   nothing happens.
   
   User must make sure that string is declared long enough to contain
   the appended characters.
   
   Remember that one position is always required for the terminating
   null.  If string is declared as string[5], then the maximum value
   for len is 4.
   
   Returns true.  No detectable errors possible.
   
   Written: 09/08/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

#define TRUE 1
#define FALSE 0

int strpad(string,ch,len)
     char *string;
     int  ch, len;
{
  int  ilen;
  ilen=strlen(string);
  while (ilen < len)
    strinsert(string,ch,ilen++);
  return TRUE;
}
