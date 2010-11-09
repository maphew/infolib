/* stroverlay.c          A function in misclib.
   ------------------------------------------------------------------------
   int stroverlay(string,substring,pos)
   char *string, *substring;
   int  pos;
   
   Overlays substring onto string at position pos.  If pos less than
   zero, then the substring will be concatenated to the string.  If
   pos is beyond the end of the string, the string will be padded with
   blanks to the length required before concatenating the substring.
   
   If the string is empty, and pos is less than or equal to zero, then
   the substring will simply be copied into it.  If pos is greater
   than zero, then blanks will be appended before copying.
   
   If the substring is empty, then there will be no affect on the
   string except to pad with blanks if pos is greater than the length
   of the string.
   
   User must make sure string is big enough to contain substring.
   Returns true.  No detectable errors possible.
   
   Written: 10/28/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

#define TRUE 1
#define FALSE 0

int stroverlay(string,substring,pos)
     char *string, *substring;
     int  pos;
{
  int  ilen, addnull;
  
  
  ilen=strlen(string);
  
  
  /* Check if we're going to need to append a null. */
  
  if (pos < 0 || pos+strlen(substring) >= ilen)
    addnull=TRUE;
  else
    addnull=FALSE;
  
  
  /* If pos is less than 0 then set pos to end of string for concatenating. */
  
  if (pos < 0)
    pos = ilen;
  
  
  /* If pos is beyond the end of the string, pad with blanks to length required. */
  
  while (ilen < pos)
    strinsert(string,' ',ilen++);
  
  
  /* Overlay substring at pos. */
  
  while (*substring != '\0')
    *(string + pos++) = *substring++;
  
  
  /* Add the null if needed. */
  
  if (addnull)
    *(string + pos) = '\0';
  
  return TRUE;
}
