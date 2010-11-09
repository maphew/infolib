/* alb2dd.c            A function in maplib.
   ------------------------------------------------------------------------
   long alb2dd(x,y,ddlon,ddlat)
   double *ddlat, *ddlon;
   double x, y;
   
   Projects a albers coordinate pair to decimal degrees.
   
   Uses the formulas given in "Map Projections Used by the
   U.S. Geological Survey" John P, Snyder, Second edition, 1984.
   
   Aggrees with ARC/INFO +- 0.001 meter.
   
   IMPORTANT: Requires math.h for trig functions.
   
   Example:
   double x = 79497.6984;
   double y = 27615.4616;
   double ddlon, ddlat;
   alb2dd(x,y,&ddlog,&ddlat);
   
   Written: 06/25/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   10/25/94 -- Added NAD83.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define TRUE 1
#define FALSE 0
#undef DEBUGGING

long alb2dd(x,y,ddlon,ddlat)
     double *ddlat, *ddlon;
     double x, y;
{
  long   done, i;
  double a, b, e, phi1, phi2, phi0, lambda0, n, c, rho0;
  double newphi, phi, cosphi, sinphi, esp, espx, q1e2, e2r, es2c;
  double     q0,          sinphi0, esp0, espx0;
  double m1, q1, cosphi1, sinphi1, esp1, espx1;
  double m2, q2, cosphi2, sinphi2, esp2, espx2;
  double rad2deg, deg2rad, pi;
  double q, rho, theta;
  int    datum;
  
  
  /* Set projection parameters.  Change for your preferred regional albers.  */
  
  phi1 = 43.5;        /* First standard parallel. */
  phi2 = 47.5;        /* Second standard parallel. */
  phi0 = 41.75;       /* Latitude of origin. */
  lambda0 = -118.0;   /* Central meridian. */
  datum = 27;         /* Use either 27 or 83 for NAD27 or NAD83. */
  
  
  /* Projection and conversion constants.   Don't change this! */
  
  switch (datum)
    {
    case 27:
      a = 6378206.4;      /* Clarke 1866 a axis (equatorial) radius. */
      b = 6356583.8;      /* Clarke 1866 b axis (polar) radius. */
      break;
    case 83:
      a = 6378137.0;      /* GRS 1980 a axis (equatorial) radius. */
      b = 6356752.3;      /* GRS 1980 b axis (polar) radius. */
      break;
    }
  e = sqrt(1-(b*b)/(a*a));     /* Eccentricity of the elipsoid. */
  pi = 3.14159265358979323846; /* I'll take pecan. */
  deg2rad = pi / 180;          /* Number of degrees per radian. */
  rad2deg = 180 / pi;          /* Number of radians per degree. */
  
#ifdef DEBUGGING
  printf("Projection parameters and conversion constants:\n");
  printf("        a=%f\n",a);
  printf("        b=%f\n",b);
  printf("        e=%f\n",e);
  printf("     phi1=%f\n",phi1);
  printf("     phi2=%f\n",phi2);
  printf("     phi0=%f\n",phi0);
  printf("  lambda0=%f\n",lambda0);
#endif
  
  
  /* Set factors that depend on other constants and each other.
     Since these don't depend on x and y, they could be hard coded
     for a specific set of projection parameters.
     
     Since sin, cos, etc expect angle in radians, need to 
     multiply angles by deg2rad to convert to radians.
     Similarly, asin, acos, etc return an angle in radians,
     need to multiply by rad2deg to convert to degrees.
     
     To keep the equations smaller and faster, a few frequently used
     factors are calculated first. */
  
  cosphi1=cos(deg2rad*phi1);
  cosphi2=cos(deg2rad*phi2);
  
  sinphi0=sin(deg2rad*phi0);
  sinphi1=sin(deg2rad*phi1);
  sinphi2=sin(deg2rad*phi2);
  
  esp0=e*sinphi0;
  esp1=e*sinphi1;
  esp2=e*sinphi2;
  
  espx0=esp0*esp0;
  espx1=esp1*esp1;
  espx2=esp2*esp2;
  
  m1=cosphi1/sqrt(1-espx1);
  m2=cosphi2/sqrt(1-espx2);
  
  q0=(1-e*e)*(sinphi0/(1-espx0)-(1/(2*e))*log((1-esp0)/(1+esp0)));
  q1=(1-e*e)*(sinphi1/(1-espx1)-(1/(2*e))*log((1-esp1)/(1+esp1)));
  q2=(1-e*e)*(sinphi2/(1-espx2)-(1/(2*e))*log((1-esp2)/(1+esp2)));
  
  n=((m1*m1)-(m2*m2))/(q2-q1);
  c=(m1*m1)+(n*q1);
  rho0=a*sqrt(c-n*q0)/n;
  
#ifdef DEBUGGING
  printf("Map constants:\n");
  printf("       n=%f\n",n);
  printf("       c=%f\n",c);
  printf("    rho0=%f\n",rho0);
#endif
  
  
  /* Set intermediate factors.  These do depend on x and y. */
  
  rho=sqrt(x*x+(rho0-y)*(rho0-y));
  theta=rad2deg*atan(x/(rho0-y));
  q=(c-((rho*rho)*(n*n)/(a*a)))/n;
  
#ifdef DEBUGGING
  printf("Intermediate results:\n");
  printf("       q=%f\n",q);
  printf("     rho=%f\n",rho);
  printf("   theta=%f\n",theta);
#endif
  
  
  /* Set longitude.  This is a cinch compared to latitude. */
  
  *ddlon = lambda0+theta/n;
  
  
  /* Set latitude.
     Here we have to iterate until the difference between phi and
     newphi is negligible, see Snyder, p 98. */
  
  /* Initialize loop. */
  
  done=FALSE;
  phi=rad2deg*asin(q/2);  /* Initial phi as per Snyder, p 96. */
  
  /* Set factors that do not change within the loop. */
  
  q1e2=q/(1-e*e);
  e2r=1/(2*e);
  
  /* Loop to calculate newphi until change is negligible.
     Test runs indicate that difference must be less than or
     equal to 0.000000001 in order to get longitude to agree
     with that produced by arc/info.  At this tolerance, my
     result agrees with theirs to the nearest 100th of a second.
     This requires between 800 and 900 iterations! */
  
  while (!done)
    {
      cosphi=cos(deg2rad*phi);
      sinphi=sin(deg2rad*phi);
      esp=e*sinphi;
      espx=esp*esp;
      es2c=((1-espx)*(1-espx))/(2*cosphi);
      newphi=phi+(es2c)*(q1e2-(sinphi/(1-espx))+(e2r*log((1-esp)/(1+esp))));
      if (newphi-phi<=0.000000001)
        done=TRUE;
      phi=newphi;
    }
  *ddlat=(phi);
  
  
  /* All done.  Get out. */
  
  return 1;
  
}
