/* strisnum.c            A function in misclib.
   ------------------------------------------------------------------------
   int strisnum(s)
   char *s;
   
   Returns true if the string is a number, else false.
   Must be valid integer, real, or E format value.  See strtype.
   
   Written: 06/30/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#define TRUE 1
#define FALSE 0
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

int strisnum(s)
     char *s;
{
  int i;
  i=strtype(s);
  if (i == -1 || i == -2 || i == -3)
    return TRUE;
  else
    return FALSE;
}
