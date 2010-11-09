/* strentry.c            A function in misclib.
   ------------------------------------------------------------------------
   char *strentry(path,entry)
   char *path, *entry;
   
   Returns the entry portion of a path.  The separator between the path
   and the entry is not returned, eg, "/tree/branch/leaf" = "leaf".
   
   Under DOS, separator is "\".
   
   Returns pointer to entry, so can be used as an intermediary to feed
   char arguments to functions that expect strings.
   
   Example:
   strentry(fullpath,entry);
   strcpy(filename,strentry(fullpath,entry))
   
   Written: 09/04/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

char *strentry(path,entry)
     char *path, *entry;
{
  char syssep[2];
  int  pos;
  
  *entry='\0';
  syssep[0]=SYSSEP;
  syssep[1]='\0';
  
  pos=strrfind(path,syssep,-1);
  if (pos >= 0)
    pos++;      /* Set pos to character after last separator. */
  else
    pos=0;      /* Path has no separator.  Return entire path. */
  
  strsubstr(path,entry,pos,strlen(path));
  
  return(entry);
}
