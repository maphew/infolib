/* dbfputrec.c
   ------------------------------------------------------------------------
   A function in dbflib.

   Writes a data record to a dbf file.
   
   Written: 09/30/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H

#define TRUE 1
#define FALSE 0

int dbfputrec(ioout,orec,recsize)
     
     FILE           *ioout;
     char           *orec;
     unsigned short recsize;
     
{
  unsigned char  byte;
  unsigned long  curbyte;
  
  
  /* Write the data record to the file. */
  
  for (curbyte=0; curbyte < recsize; curbyte++)
    {
      byte=*(orec+curbyte);
      if (fwrite(&byte,sizeof(char),1,ioout) != sizeof(char))
        return DBF_WRITE_DATA;
    }
  
  
  /* Done. */
  
  return DBF_OK;
}
