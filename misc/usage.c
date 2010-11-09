/* usage.c               A function in misclib.
   ------------------------------------------------------------------------
   int usage(progname,progver,progargs,authorid)
   char *progname, *progver, *progargs;
   int  authorid;
   
   Gives usage, version, copyright, and author information.
   
   Accepts string values for the name of a program, its arguments, and its
   current version, and prints out a standard usage message.
   
   If authorid is 0, then author identification will NOT be printed.
   If authorid is 1, then author identification will also be printed.
   If authorid is 2, then ONLY the author identification will be printed.
   
   Written: 04/09/93 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   06/17/94 Added idonly option.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int usage(progname,progver,progargs,authorid)
     char *progname, *progver, *progargs;
     int  authorid;
{
  if (authorid != 2)
    printf("%s Version %s.\n",progname,progver);
  
  if (authorid > 0)
    {
      printf("Randy Deardorff, US EPA Region 10, Seattle.");
      printf(" (deardorff.randy@epa.gov)\n");
    }
  
  if (authorid != 2)
    {
      strlwr(progname);
      printf("Usage: %s %s\n",progname,progargs);
    }
  
  return 1;
}
