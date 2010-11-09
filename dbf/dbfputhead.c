/* dbputhead.c
   ------------------------------------------------------------------------
   A function in dbflib.

   Writes the header section of a dbf file.
   Note byte swapping of integer values.
   
   Written: 09/30/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes by the author unless otherwise noted)
   
   07/20/94 Added logic to check for and handle various sizeofs for
   other systems as much as possible.

   07/30/94 Moved unions from system header to here.

   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H
#include DBFSYS_H

#define TRUE 1
#define FALSE 0

int dbfputhead(ioout,dbfid,month,day,year,dataoffset,recsize,lastrec)
     
     FILE           *ioout;
     unsigned char  dbfid, month, day, year;
     long           lastrec;
     unsigned short dataoffset, recsize;
     
{
  unsigned char  byte, iswap[3];
  unsigned long  curbyte;
  union          dbfunion dbfswap;
  union          header head;
  union          fieldrec field;
  
  
  /* Build the header structure. */
  
  memset(head.headch,0,DBF_BUFFSIZE);  /* Fill with nulls. */
  head.headst.dbf_id=dbfid;
  head.headst.last_update[0]=year;
  head.headst.last_update[1]=month;
  head.headst.last_update[2]=day;

  if (sizeof(unsigned short)==2)
    dbfswap.ushortbuff=recsize;
  else
    if (sizeof(unsigned)==2)
      dbfswap.uintbuff=recsize;
    else
      return DBF_BAD_SIZEOF;
  head.headch[10]=dbfswap.charbuff[0];
  head.headch[11]=dbfswap.charbuff[1];

  if (sizeof(unsigned short)==2)
    dbfswap.ushortbuff=dataoffset;
  else
    if (sizeof(unsigned)==2)
      dbfswap.uintbuff=dataoffset;
    else
      return DBF_BAD_SIZEOF;
  head.headch[8]=dbfswap.charbuff[0];
  head.headch[9]=dbfswap.charbuff[1];
  
  if (sizeof(long)==4)
    dbfswap.lintbuff=lastrec;
  else
    if (sizeof(int)==4)
      dbfswap.intbuff=lastrec;
    else
      return DBF_BAD_SIZEOF;
  head.headch[4]=dbfswap.charbuff[0];
  head.headch[5]=dbfswap.charbuff[1];
  head.headch[6]=dbfswap.charbuff[2];
  head.headch[7]=dbfswap.charbuff[3];
  
  
  /* Swap byte order of integer fields. */
  
#if DOS_SWAP
  
  iswap[0]=head.headch[4];
  iswap[1]=head.headch[5];
  iswap[2]=head.headch[6];
  iswap[3]=head.headch[7];
  head.headch[4]=iswap[3];
  head.headch[5]=iswap[2];
  head.headch[6]=iswap[1];
  head.headch[7]=iswap[0];
  
  iswap[0]=head.headch[8];
  iswap[1]=head.headch[9];
  head.headch[8]=iswap[1];
  head.headch[9]=iswap[0];
  
  iswap[0]=head.headch[10];
  iswap[1]=head.headch[11];
  head.headch[10]=iswap[1];
  head.headch[11]=iswap[0];
  
#endif
  
  
  /* Write the structure to the output file. */
  
  for (curbyte=0; curbyte < sizeof(head.headst); curbyte++)
    {
      byte=head.headch[curbyte];
      if (fwrite(&byte,sizeof(char),1,ioout) != sizeof(char))
        return DBF_WRITE_HEAD;
    }
  
  
  /* Done. */
  
  return DBF_OK;
}
