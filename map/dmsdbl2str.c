/* dmsdbl2str.c        A function in maplib.
   ------------------------------------------------------------------------
   int dmsdbl2str(dms,str,len,numdecs)
   double dms;
   char   *str;
   int    len, numdecs;
   
   Accepts a packed dms coordinate as a double, and returns a packed
   dms string coordinate.  Len is the total length to write, including
   the decimal portion and the decimal point itself.
   
   If seconds if greater or equal to 59.99, then it rolls over as a
   full minute.  Degrees will rollover also if minutes was already 59.
   
   Example:
   int    len=9;
   int    numdecs=1;
   char   chcoord[10];
   double coord = 1223023.99;
   ok=dmsdbl2str(coord,chcoord,len,numdecs);
   
   Written: 06/08/93 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MAP_H
#include MISC_H

int dmsdbl2str(dms,str,len,numdecs)
     double dms;
     char   *str;
     int    len, numdecs;
     
{
  int     ok;
  long    deg, min;
  double  sec, dmscoord, maxsec;
  
  ok=dmsdbl2int(dms,&deg,&min,&sec);
  
  if (!(ok))
    return ok;
  
  while (sec>=60)
    {
      min++;
      sec=sec-60;
    }
  
  switch (numdecs)
    {
    case 1:
      maxsec=59.9;
      break;
    case 2:
      maxsec=59.99;
      break;
    case 3:
      maxsec=59.999;
      break;
    default:
      maxsec=59.9999;
      break;
    }
  
  if (sec>=maxsec)
    {
      min++;
      sec=0;
    }
  
  while (min>=60)
    {
      if (deg>0)
        deg++;
      else
        deg--;
      min=min-60;
    }
  
  dmscoord=deg*10000+min*100+sec;
  sprintf(str,"%*.*f",len-numdecs-1,numdecs,dmscoord);
  return 1;
}
