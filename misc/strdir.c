/* strdir.c              A function in misclib.
   ------------------------------------------------------------------------
   char *strdir(path,dir)
   char *path, *dir;
   
   Returns the directory portion of a path.  The separator between the
   path and the entry is not returned, eg, "/tree/branch/leaf" =
   "/tree/branch".
   
   Under DOS, separator is "\" and path will include drive letter &
   colon.
   
   Returns pointer to dir, so can be used as an intermediary to feed
   char arguments to functions that expect strings.
   
   Example:
   strdir(fullpath,dir);
   strcat(strdir(fullpath,dir),"/foobar.tmp")
   
   Written: 09/03/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

char *strdir(path,dir)
     char *path, *dir;
{
  char syssep[2];
  int  possep;
  
  *dir='\0';
  syssep[0]=SYSSEP;
  syssep[1]='\0';
  
  possep=strrfind(path,syssep,-1);
  
  if (possep > 0)
    strsubstr(path,dir,0,possep);
  if (possep == 0)
    strcpy(dir,syssep);
  
  return(dir);
}
