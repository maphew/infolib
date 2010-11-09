/* dmsstr2dbl.c        A function in maplib.
   ------------------------------------------------------------------------
   int dmsstr2dbl(str,dbl)
   char   *str;
   double *dbl;
   
   Converts a string dms to a packed double dms.  Returns true if ok,
   else error.  String can be packed (eg 1234567.89), or separated by
   space (eg 123 45 67.89), or separated by colons (eg 123:45:67.89).
   
   Example:
   char   *coord = "123:45:67.89";
   double x;
   int    ok;
   ok=dmsstr2dbl(coord,&x);
   
   Written: 07/01/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MAP_H
#include MISC_H

#define TRUE 1
#define FALSE 0

int dmsstr2dbl(str,dbl)
     char   *str;
     double *dbl;
{
  char   temp[20];
  long   deg, min;
  double sec;
  
  /* Make sure str is not empty. */
  
  if (*str == '\0')
    return FALSE;
  
  /* Replace any colons with spaces. */
  
  stredt(str,':',' ');
  
  /* Convert to double. */
  
  if (strfind(str,ctoa(' ',temp),0) >= 0)
    
    /* String contains a space.  Assume separate values. */
    
    {
      strext(str,1,temp,' ');
      if (strisnum(temp))
        deg=atoi(temp);
      else
        return FALSE;
      strext(str,2,temp,' ');
      if (strisnum(temp))
        min=atoi(temp);
      else
        return FALSE;
      strext(str,3,temp,' ');
      if (strisnum(temp))
        sec=atof(temp);
      else
        return FALSE;
      *dbl=dmsint2dbl(deg,min,sec);
    }
  else
    
    /* String contains no space.  Must already be packed. */
    
    {
      if (strisnum(str))
        *dbl=atof(str);
      else
        return FALSE;
    }
  
  /* Success! */
  
  return TRUE;
}
