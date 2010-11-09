/* map.h
   ------------------------------------------------------------------------
   This header declares the functions kept in maplib.a.
   
   Written: 06/25/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   07/29/94 -- Made longs of some ints for IBM PC.
   
   ------------------------------------------------------------------------ */

long   alb2dd();
long   dd2alb();
double dd2dms();
double dms2dd();
int    dmsdbl2int();
double dmsint2dbl();
int    dmsstr2dbl();
int    dmsdbl2str();
int    dmsunpack();
