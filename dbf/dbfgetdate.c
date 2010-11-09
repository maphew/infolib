/* dbfgetdate.c
   ------------------------------------------------------------------------
   A function in dbflib.

   Sets the date terms to today's date.
   
   Written: 10/07/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H

#define TRUE 1
#define FALSE 0

int dbfgetdate(month,day,year)
     
     unsigned char  *month, *day, *year;
     
{
  char    timebuff[26];
  char    buff[4];
  char    *monthlist="JAN!FEB!MAR!APR!MAY!JUN!JUL!AUG!SEP!OCT!NOV!DEC";
  short   m, d, y;
  long    current_time=time(0);
  
  strcpy(timebuff,ctime(&current_time));
  timebuff[24]='\0'; /* Change newline to a null. */
  
  /* Format of timebuff:
     
     000000000011111111112222
     012345678901234567890123
     Wed Oct  7 08:37:47 1992
     
     */
  
  strsubstr(timebuff,buff,4,3);
  strupr(buff);
  m=strkeyword(monthlist,buff,'!',"left");
  if (m < 1 || m > 12)
    return DBF_GET_DATE;
  
  strsubstr(timebuff,buff,8,2);
  strtrim(buff,"both",' ');
  if (strisnum(buff))
    {
      d=atoi(buff);
      if (d < 1 || d > 31)
        return DBF_GET_DATE;
    }
  else
    return DBF_GET_DATE;
  
  strsubstr(timebuff,buff,22,2);
  strtrim(buff,"both",' ');
  if (strisnum(buff))
    {
      y=atoi(buff);
      if (y < 0 || y > 99)
        return DBF_GET_DATE;
    }
  else
    return DBF_GET_DATE;
  
  *month=m;
  *day=d;
  *year=y;
  return DBF_OK;
}
