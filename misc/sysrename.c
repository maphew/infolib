/* sysrename.c           A function in misclib.
   ------------------------------------------------------------------------
   int  sysrename(file,newname)
   char *file, *newname;
   
   Renames the file to newname.  Returns true if ok, else false.
   
   Under unix, if the user does not have write permission to the file,
   the file will not be renamed, but the routine will return true.
   
   Written: 09/22/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   02/03/95 Minor changes for DJGPP_DOS.
   
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

#define TRUE 1
#define FALSE 0

int  sysrename(file,newname)
     char *file, *newname;
{
  
  char  cmd[256];
  int   retval, status;
  
  retval=FALSE;
  
  
  /* Do nothing unless the file exists! */
  
  if (sysexist(file))
    {
      
      
      /* Delete the file named newname if it exists. */
      
      if (sysexist(newname))
        {
          if (!(sysdel(newname)))
            return FALSE;
        }
      
      /* Ok, now rename the thing. */
      
#if BORLAND_C | DJGPP_DOS
      
      if (rename(file,newname))
        retval=0;
      else
        retval=1;
      
#else
     
      strcpy(cmd,"mv ");
      strcat(cmd,file);
      strcat(cmd," ");
      strcat(cmd,newname);
      status=system(cmd);
      if (status==0)
        retval=1;
      
#endif
      
    }
  
  return retval;
  
}
