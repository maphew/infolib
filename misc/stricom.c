/* stricom.c             A function in misclib.
   ------------------------------------------------------------------------
   int stricom(str1,str2)
   char   *str1, *str2;
   
   Compares two strings.  Either string can be shorter or longer.
   Either or both can be empty.  If both are empty, then the match is
   perfect.  If just one is empty, then the match is a total mismatch.
   
   Return values:
   
   -1   Total mismatch.  The strings do not match at all.
   0    Perfect match.  The strings match perfectly.
   n    Match up to and including position n.  Example "FOO","FOOBAR" = 3.
   
   Written: 09/01/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

#define PERFECTMATCH 0
#define TOTALMISMATCH -1

int stricom(str1,str2)
     char   *str1, *str2;
{
  int curpos, len1, len2;
  len1=strlen(str1);
  len2=strlen(str2);
  
  
  /* Loop over strings comparing until either ends, or missmatch occurs. */
  
  curpos=0;
  while (*str1 != '\0' && *str2 != '\0')
    {
      while (*str2 != '\0')
        {
          
          /* Check if characters at current position match. */
          
          if (*str1 != *str2)
            
            /* Characters at current position do not match. */
            
            {
              
              /* If looking at position zero, return total mismatch. */
              
              if (curpos == 0)
                return TOTALMISMATCH;
              
              /* Otherwise strings match up to curpos.  */
              
              else
                return curpos;
            }
          
          /* Characters at current position do match:  keep scanning. */
          
          curpos++;
          *str1++;
          *str2++;
        }
    }
  
  
  /* At this point, we have reached the end of one or both of the strings.
     Thus, the strings match perfectly up to the length of the shorter one,
     or, if strings are the same length, then they are a perfect match. */
  
  
  /* Check for perfect match.  This includes both being empty. */
  
  if (len1 == len2)
    return PERFECTMATCH;
  
  
  /* If one is empty and the other is not, then total mismatch. */
  
  if (len1 == 0 || len2 == 0)
    return TOTALMISMATCH;
  
  
  /* Must be match to end of shorter string. */
  
  if (*str2 == '\0')
    return curpos;
  else
    return curpos+1;
  
}
