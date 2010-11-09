/* syscwd.c              A function in misclib.
   ------------------------------------------------------------------------
   char *syscwd(str)
   char *str;
   
   Returns the current working directory in str.  The value is
   determined simply by looking up the current value of the
   environmental variable PWD.  A final slash is NOT appended.
   Example return: "/usr23/rbd/c/misc".
   
   Returns pointer to str, so can be used as an intermediary to feed
   char arguments to functions that expect strings.
   
   Example:
   syscwd(curdir);
   if (strcmp(syscwd(curdir),"/") == 0)
   
   Written: 09/03/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   07/30/94 Renamed from getcwd for compatibility with Borland C.

   02/03/95 Minor changes for DJGPP_DOS.

   04/25/95 Now use getcwd on all systems.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>

#if BORLAND_C
#include <dir.h>
#endif

#if DJGPP_DOS
#include <dir.h>
#include <std.h>
#endif

#if ANY_UNIX
#include <sys/param.h>
#endif

char *syscwd(str)
     char *str;
{
  
#if ANY_UNIX
  
  char buffer[MAXPATHLEN];
  getcwd(buffer,MAXPATHLEN);
  strcpy(str,buffer);
  
#else
  
  char buffer[MAXPATH];
  getcwd(buffer,MAXPATH);
  strcpy(str,buffer);
  
#endif
  
  return(str);
}
