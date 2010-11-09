/* strquote.c            A function in misclib.
   ------------------------------------------------------------------------
   int strquote(str)
   char   *str;
   
   Quotes str by enclosing it in single quotes.  Str may be empty.  If
   so, it will end up consisting of two single quotes.  User must
   insure that str is defined long enough to include the quotes.
   
   Written: 07/15/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

int strquote(str)
     char   *str;
{
  strinsert(str,'\'',strlen(str));
  strinsert(str,'\'',0);
  return 1;
}
