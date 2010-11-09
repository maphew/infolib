/* sysexist.c            A function in misclib.
   ------------------------------------------------------------------------
   int  sysexist(filename)
   char *filename;
   
   Returns true if the file exists and can be opened for reading, else
   false.
   
   Written: 09/22/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   02/03/95 Minor changes for DJGPP_DOS.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

#if ANY_UNIX
#include <sys/file.h>
#endif

#if BORLAND_C
#include <io.h>
#endif

#if DJGPP_DOS
#include <dir.h>
#include <std.h>
#endif

#ifndef F_OK
#define F_OK 0
#endif

int sysexist(filename)
     char *filename;
{
  FILE *io;
  if (access(filename,F_OK)==0)
    return 1;
  else
    return 0;
}
