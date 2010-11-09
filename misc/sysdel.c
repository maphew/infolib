/* sysdel.c              A function in misclib.
   ------------------------------------------------------------------------
   int  sysdel(filename)
   char *filename;
   
   Deletes the file filename.  Returns true if ok, else false.
   
   Under unix, if the user does not have write permission to the file,
   the file will not be deleted, but the routine will return true.
   
   Written: 09/22/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   02/03/95 Minor changes for DJGPP_DOS.
   
   04/25/95 Use unlink on any unix.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

#if DJGPP_DOS
#include <dir.h>
#include <std.h>
#endif

int sysdel(filename)
     char *filename;
{
  
  int   retval=0;

#if ANY_UNIX

  if (unlink(filename))
    retval=0;
  else
    retval=1;
  
#else
  
  if (remove(filename))
    retval=0;
  else
    retval=1;
  
#endif

  return retval;
  
}
