/* dmsdbl2int.c        A function in maplib.
   ------------------------------------------------------------------------
   int dmsdbl2int(dms,deg,min,sec)
   double dms, *sec;
   long   *deg, *min;
   
   Accepts a packed dms coordinate as a double, and returns separate
   numeric values for degrees, minutes and seconds.
   
   Example:
   int    ok;
   long   deg, min;
   double sec;
   double coord = 1223023.15;
   ok=dmsdbl2int(coord,&deg,&min,&sec)
   
   Written: 07/21/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

int dmsdbl2int(dms,deg,min,sec)
     double dms, *sec;
     long   *deg, *min;
{
  long   d, m;
  double s;
  d=dms/10000;
  m=(dms-(d*10000))/100;
  s=dms-(d*10000)-(m*100);
  *deg=d;
  *min=m;
  *sec=s;
  return 1;
}
