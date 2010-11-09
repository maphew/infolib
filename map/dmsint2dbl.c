/* dmsint2dbl.c        A function in maplib.
   ------------------------------------------------------------------------
   double dmsint2dbl(deg,min,sec)
   long   deg, min;
   double sec;
   
   Convert separate numbers for degrees minutes and seconds to a
   packed double.  If seconds if greater or equal to 59.9995, then it
   rolls over as a full minute.  ARC/INFO rounds decimal degrees to
   nearest 4th decimal place, i.e., 59.9995.  Degrees will rollover
   also if minutes was already 59.
   
   Example:
   double coord;
   long   deg = 122;
   long   min = 30;
   double sec = 23.15;
   coord=dmsint2dbl(deg,min,sec);
   
   Written: 07/01/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   4/20/95 -- Fixed bug that caused errors with negative longs.
   
   ------------------------------------------------------------------------ */

double dmsint2dbl(deg,min,sec)
     long   deg, min;
     double sec;
{
  double retval;
  while (sec>=60)
    {
      min++;
      sec=sec-60;
    }
  if (sec>=59.9995)
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
  
  if (deg < 0)
    retval=0-(abs(deg)*10000+min*100+sec);
  else
    retval=deg*10000+min*100+sec;

  return retval;
}
