/* dd2dms.c            A function in maplib.
   ------------------------------------------------------------------------
   double dd2dms(dd)
   double dd;
   
   Converts decimal degrees as double to degrees minutes seconds as
   packed double.
   
   Example:
   double dms;
   double dd = 45.2369;
   dms=dd2dms(dd);
   
   Written: 06/26/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   03/27/96 Fixed a bug to do with negative longs.
   
   ------------------------------------------------------------------------ */

#include <rbdpaths.h>
#include MAP_H

double dd2dms(dd)
     double dd;
{
  int    wasneg;
  long   deg, min;
  double sec, ddt, retval;

  if (dd < 0)
    {
      wasneg=1;
      ddt=-dd;
    }
  else
    {
      wasneg=0;
      ddt=dd;
    }
    
  deg=ddt;
  min=(ddt-deg)*60;
  sec=((ddt-deg)*3600)-min*60;
  if (wasneg)
    deg=-deg;
  
  retval=dmsint2dbl(deg,min,sec);
  return retval;
}
