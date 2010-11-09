/* strqquote.c           A function in misclib.
   ------------------------------------------------------------------------
   int strqquote(str,quotechar)
   char   *str;
   int    quotechar;
   
   Quotes str by enclosing it in the specified quote character.  Str
   may be empty.  If so, it will end up consisting of two quotes
   characters.  User must insure that str is defined long enough to
   include the added characters.
   
   Written: 07/08/93 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

int strqquote(str,quotechar)
     char   *str;
     int    quotechar;
{
  strinsert(str,quotechar,strlen(str));
  strinsert(str,quotechar,0);
  return 1;
}
