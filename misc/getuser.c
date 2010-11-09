/* getuser.c             A function in misclib.
   ------------------------------------------------------------------------
   char *getuser(str)
   char *str;
   
   Returns the user's login name in str.  The value is determined
   simply by looking up the current value of the environmental
   variable USER.
   
   Returns pointer to str, so can be used as an intermediary to feed
   char arguments to functions that expect strings.
   
   Example:
   getuser(username);
   if (strcmp(getuser(username),"hmonoyer") != 0)
   
   Written: 09/03/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *getuser(str)
     char *str;
{
  strcpy(str, getenv ("USER"));
  return(str);
}
