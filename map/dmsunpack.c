/* dmsunpack.c         A function in maplib.
   ------------------------------------------------------------------------
   int dmsunpack(str,dlm)
   char  *str
   int    dlm
   
   Unpacks a string dms value to unpacked string separated by dlm.
   String can be packed (eg 1234567.89), or separated by space (eg 123
   45 67.89), or separated by colons (eg 123:45:67.89).  Returns true
   if ok, else error.
   
   Example:
   char   coord[14];
   int    ok;
   strcpy(coord,"1234567.89");
   ok=dmsunpack(coord,':');
   
   Written: 08/11/92 Randy Deardorff.  USEPA Seattle.
   
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

int dmsunpack(str,dlm)
     char  *str;
     int    dlm;
{
  long   deg, min, ok;
  double dms, sec;
  
  ok=dmsstr2dbl(str,&dms);
  if (ok)
    {
      dmsdbl2int(dms,&deg,&min,&sec);
      if (dms < 0)
        {
          min=-min;
          sec=-sec;
        }
      if (sec < 10)
        sprintf(str,"%3d%c%2d%c %2.2f",
                deg,dlm,min,dlm,sec);
      else
        sprintf(str,"%3d%c%2d%c%2.2f",
                deg,dlm,min,dlm,sec);
    }
  return ok;
}
