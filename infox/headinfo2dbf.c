/* headinfo2dbf.c
   ------------------------------------------------------------------------
   A function in infoxlib.
   
   Converts the header terms of the INFO file to appropriate header terms for
   the dbf file.
      
   Written: 10/06/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   08/30/94 Rewrote from heade002dbf.
   11/09/94 Changed *hasmemo from int to unsigned short.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H
#include DBFSYS_H
#include INFOLIB_H
#include INFO_H

#define TRUE 1
#define FALSE 0

int headinfo2dbf(numrecs,numfields,recsize,dbfid,month,day,year,
                dataoffset,lastrec,filesize,hasmemo)
     
     long int       numrecs;
     unsigned long  numfields;
     unsigned long  *filesize;
     unsigned char  *dbfid, *month, *day, *year;
     long           *lastrec;
     unsigned short *hasmemo, *dataoffset, recsize;
     
{
  *dbfid=3;            /* Always 3.  Indicates no memo fields. */
  *hasmemo=FALSE;
  
  *year=85;            /* Set to today from main by calling dbfgetdate. */
  *month=7;
  *day=29;
  
  *lastrec=numrecs;
  
  /* Calculate the data offset. */
  
  *dataoffset=(numfields * DBF_BUFFSIZE)+DBF_BUFFSIZE+1;
  
  
  /* Calculate file size. */
  
  *filesize=(recsize * numrecs) + *dataoffset + 1;
  
  /* Done. */
  
  return INFO_OK;
}
