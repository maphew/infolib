/* dbputtail.c
   ------------------------------------------------------------------------
   A function in dbflib.

   Writes the tail section of a dbf file.
   This is nothing nore that a control z.
   
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

int dbfputtail(ioout)
     
     FILE *ioout;
     
{
  putc(26, ioout);
  return DBF_OK;
}
