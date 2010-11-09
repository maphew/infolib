/* sysdate.c             A function in misclib.
   ------------------------------------------------------------------------
   int sysdate(format,datebuff)
   char *format, *datebuff;
   
   Returns the current date in the format specified.
   
   Valid formats are:
   FULL         -- Full date and time as returned by ctime.
   YYYYMMDD     -- 19930120
   YYMMDD       -- 930120
   DD-MMM-YYYY  -- 20-JAN-1993
   MM/DD/YY     -- 01/20/93
   FTAG         -- File name fragment, based on date and time, in format
                   "YMDHmm.ss" where Y, M, D, and H are single characters for
                   the current year, month, day, and hour, and MM, and SS are
                   two digit integers representing the current year, minute,
                   and seconds.  Should work in DOS or UNIX.
   FILE         -- Same as FTAG, but no dot, just an 8 character filename.
   
   Written: 01/20/93 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   12/27/94 -- Added ftag option.
   02/01/94 -- Added full option.
   06/16/97 -- Added file option and changed ftag format slightly.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

#define TRUE 1
#define FALSE 0

int sysdate(format,datebuff)
     char  *format, *datebuff;
     
{
  char     timebuff[26];
  char     buff[5], filename[9];
  int      m, d, y, c, hour, min, sec, status;
  char     month[4];
  long     current_time=time(0);
  
  
  /* Establish a look up list for month names. */
  
  char *monthlist="JAN!FEB!MAR!APR!MAY!JUN!JUL!AUG!SEP!OCT!NOV!DEC";
  
  
  /* Establish a look up list for output formats. */
  
  char *formatlist="YYYYMMDD!YYMMDD!DD-MMM-YYYY!MM/DD/YY!FULL!FTAG!FILE";
  
  
  /* Build a list of ASCII characters for ftag conversions. */
  
  char *charlist="123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  
  
  /* Get time/date string from operating system.
     
     000000000011111111112222
     012345678901234567890123
     Wed Oct  7 08:37:47 1992
     */
  
  strcpy(timebuff,ctime(&current_time));
  timebuff[24]='\0'; /* Change newline to a null. */
  
  /* Extract month. */
  
  strsubstr(timebuff,month,4,3);
  strupr(month);
  m=strkeyword(monthlist,month,'!',"left");
  if (m < 1 || m > 12)
    return FALSE;
  
  
  /* Extract day. */
  
  strsubstr(timebuff,buff,8,2);
  strtrim(buff,"both",' ');
  if (strisnum(buff))
    {
      d=atoi(buff);
      if (d < 1 || d > 31)
        return FALSE;
    }
  else
    return FALSE;
  
  
  /* Extract year. */
  
  strsubstr(timebuff,buff,22,2);
  strtrim(buff,"both",' ');
  if (strisnum(buff))
    {
      y=atoi(buff);
      if (y < 0 || y > 99)
        return FALSE;
    }
  else
    return FALSE;
  
  
  /* Extract century. */
  
  strsubstr(timebuff,buff,20,2);
  strtrim(buff,"both",' ');
  if (strisnum(buff))
    {
      c=atoi(buff);
      if (c < 0 || c > 99)
        return FALSE;
    }
  else
    return FALSE;
  
  
  /* Extract hour, minute, and second. */
  
  strsubstr(timebuff,buff,11,2);
  strtrim(buff,"both",' ');
  hour=atoi(buff);
  strsubstr(timebuff,buff,14,2);
  strtrim(buff,"both",' ');
  min=atoi(buff);
  strsubstr(timebuff,buff,17,2);
  strtrim(buff,"both",' ');
  sec=atoi(buff);


  /* Do some preliminary file name stuff. */
  
  strupr(format);
  switch (strkeyword(formatlist,format,'!',"left"))
    {
    case 6:
    case 7:
      
      /* Get year in range 1 to 903 where 1 is 1997 and 999 is 2899.
         1997 = 97, 2003 = 103, etc. */
      
      c=c*100;
      if (c > 1900)
        y=y+(c-1900);
      if (y > 96)
        y=y-96;
      
      /* Put year in range 1-26. */
      
      while (y > 26)
        y=y-26;
      
      /* Put hour in range 1-25 (currently 0-24). */
      
      hour++;
      break;
    }
  
  
  /* Write to output in specified format. */
  
  status=FALSE;
  switch (strkeyword(formatlist,format,'!',"left"))
    {
    case -1:        /* ambiguous */
      break;
      
    case 0:         /* not found */
      break;
      
    case 1:   /* YYYYMMDD */
      sprintf(datebuff,"%2i%2i%2i%2i",c,y,m,d);
      stredt(datebuff,' ','0');
      status=TRUE;
      break;
      
    case 2:   /* YYMMDD */
      sprintf(datebuff,"%2i%2i%2i",y,m,d);
      stredt(datebuff,' ','0');
      status=TRUE;
      break;
      
    case 3:   /* DD-MMM-YYYY */
      sprintf(datebuff,"%2i-%s-%2i%2i",d,month,c,y);
      stredt(datebuff,' ','0');
      status=TRUE;
      break;
      
    case 4:   /* MM/DD/YY */
      sprintf(datebuff,"%2i/%2i/%2i",m,d,y);
      stredt(datebuff,' ','0');
      status=TRUE;
      break;

    case 5:   /* FULL */
      strcpy(datebuff,timebuff);
      status=TRUE;
      break;
      
    case 6:   /* FTAG */
      sprintf(datebuff,"%c%c%c%c%2i.%2i",
              y+64, m+64, charlist[d-1], hour+64, min, sec);
      stredt(datebuff,' ','0');
      status=TRUE;
      break;
      
    case 7:   /* FILE */
      sprintf(datebuff,"%c%c%c%c%2i%2i",
              y+64, m+64, charlist[d-1], hour+64, min, sec);
      stredt(datebuff,' ','0');
      status=TRUE;
      break;
      
    }
  
  
  return status;
}
