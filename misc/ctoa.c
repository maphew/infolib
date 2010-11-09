/* ctoa.c                A function in misclib.
   ------------------------------------------------------------------------
   char *ctoa(ch,str)
   char ch, *str;
   
   Converts character ch to string str.
   Returns pointer to str, so can be used as an intermediary to feed char
   arguments to functions that expect strings.
   
   Example:
   pos=strfind(string,element,start);
   pos=strfind(string,ctoa('X',element),start);
   
   The mininum declared length of str (element in the example) is 2.
   
   Written: 07/16/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *ctoa(ch,str)
     char ch, *str;
{
  sprintf(str,"%c",ch);
  return(str);
}
