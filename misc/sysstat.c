/* sysstat.c             A function in misclib.
   ------------------------------------------------------------------------
   int  sysstat(filename,question)
   char *filename;
   char *question;
   
   Returns true if the question asked about the file is true, else false.
   Valid questions:
   "exists"
   "isfile"
   "isdir"
   
   Will add more later.
   
   Written: 12/16/96 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   01/14/95 -- Changes for BORLAND_C.

   ------------------------------------------------------------------------ */
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <rbdpaths.h>
#if BORLAND_C
#include <io.h>
#else
#include <sys/file.h>
#endif

#ifndef F_OK
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4
#endif

#define FALSE 0
#define TRUE 1

int sysstat(filename,question)
     char *filename;
     char *question;
{
  int retval;
  struct stat sbuf;
  retval=FALSE;

  
  if (strcmp(question,"exists") == 0)
    {
      if (access(filename,F_OK)==0)
        retval=TRUE;
    }
  else
    if (access(filename,F_OK)==0)
      {
        
        stat(filename,&sbuf);
        
        if (strcmp(question,"isdir") == 0)
          if ((sbuf.st_mode & S_IFMT) == S_IFDIR)
            retval=TRUE;
        
        if (strcmp(question,"isfile") == 0)
          if ((sbuf.st_mode & S_IFMT) == S_IFREG)
            retval=TRUE;
        
      }
  
  
  return retval;
  
}
