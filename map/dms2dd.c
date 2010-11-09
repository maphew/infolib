/* dms2dd.c            A function in maplib.
   ------------------------------------------------------------------------
   double dms2dd(dms)
   double dms;
   
   Convert packed double degrees minutes seconds to decimal degrees as
   double.
   
   Example:
   double dd;
   double dms = 1243045.23;
   dd=dms2dd(dms);
   
   Written: 06/29/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

double dms2dd(dms)
     double dms;
{
  long   deg, min;
  double sec, ddeg, dmin, dd;
  deg=dms/10000;
  min=(dms-(deg*10000))/100;
  sec=dms-(deg*10000)-(min*100);
  ddeg=deg;
  dmin=min;
  dd=deg+(dmin/60)+(sec/3600);
  return dd;
}
