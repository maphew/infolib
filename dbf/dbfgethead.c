/* dbgethead.c
   ------------------------------------------------------------------------
   A function in dbflib.

   Reads the header section of a dbf file.
   Places the header parameters into the variables passed as arguments.
   
   Filesize, numdefs, and hasmemo are not actually read from the
   header, but are implied by other items in the header, thus they are
   calculated and returned.
   
   Note byte swapping of integer values.
   
   Written: 09/25/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes by the author unless otherwise noted)

   03/02/94 Allow 5 (^E) as memo flag to indicate memos present.  This is
   apparently what dBASE 4 uses rather than 131.

   07/20/94 Added logic to check for and handle various sizeofs for
   other systems as much as possible.

   07/30/94 Moved unions from system header to here.

   09/07/94 Changed check for memo fields to recognize dbfid=3 as no memo,
   and everything else as memo.

   04/27/95 Added some type casts to eliminate nuisance compile warnings.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H
#include DBFSYS_H
#define TRUE 1
#define FALSE 0
#undef DEBUGGING

int dbfgethead(ioin,dbfid,month,day,year,dataoffset,recsize,
               lastrec,filesize,hasmemo,numfields)
     
     FILE           *ioin;
     unsigned long  *filesize, *numfields;
     unsigned char  *dbfid, *month, *day, *year;
     long           *lastrec;
     unsigned short *dataoffset, *recsize, *hasmemo;
     
{
  unsigned char  byte, iswap[3];
  unsigned long  numbytes, i;
  unsigned short trecsize;
  union          dbfunion dbfswap;
  union          header head;
  union          fieldrec field;

  
  /* Read the header section into the structure. */
  
  for (numbytes=0; numbytes < sizeof(head.headst); numbytes++)
    if (fread(&byte,sizeof(char),1,ioin) == sizeof(char))
      head.headch[numbytes]=byte;
    else
      return DBF_READ_HEAD;
  
  if (numbytes != sizeof(head.headst))
    return DBF_READ_HEAD;
  
  
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
  
  
  /* List the header to the screen byte by byte for viewing. */
  
#ifdef DEBUGGING
  for (i=0; i <= 11; i++)
    {
      byte=head.headch[i];
      printf("%d = %d %u %X\n",i,byte,byte,byte);
    }
#endif
  
  /* Copy the header structure variables to the returned variables. */
  
  *dbfid=head.headst.dbf_id;
  
  *year=head.headst.last_update[0];
  *month=head.headst.last_update[1];
  *day=head.headst.last_update[2];

  dbfswap.charbuff[0]=head.headch[10];
  dbfswap.charbuff[1]=head.headch[11];
  if (sizeof(unsigned short)==2)
    trecsize=dbfswap.ushortbuff;
  else
    if (sizeof(unsigned)==2)
      trecsize=dbfswap.uintbuff;
    else
      return DBF_BAD_SIZEOF;
  
  if (trecsize > DBF_MAXLEN)
    return DBF_REC_TOO_LONG;
  *recsize = trecsize;
  
  dbfswap.charbuff[0]=head.headch[8];
  dbfswap.charbuff[1]=head.headch[9];
  if (sizeof(unsigned short)==2)
    *dataoffset=dbfswap.ushortbuff;
  else
    if (sizeof(unsigned)==2)
      *dataoffset=dbfswap.uintbuff;
    else
      return DBF_BAD_SIZEOF;
  
  dbfswap.charbuff[0]=head.headch[4];
  dbfswap.charbuff[1]=head.headch[5];
  dbfswap.charbuff[2]=head.headch[6];
  dbfswap.charbuff[3]=head.headch[7];
  if (sizeof(long)==4)
    *lastrec=dbfswap.lintbuff;
  else
    if (sizeof(int)==4)
      *lastrec=dbfswap.intbuff;
    else
      return DBF_BAD_SIZEOF;
  
  
  /* Set memo flag. */
  
  switch (*dbfid)
    {
    case 3:
      *hasmemo=FALSE;
      break;
      /*
         case 131:  dBASE 3+ and earlier.
         case 5:    dBASE 4.
         case 245:  FoxPro 2.5
         */
    default:
      *hasmemo=TRUE;
      break;
    }
  
  
  /* Calculate file size. */
  
  *filesize=(*recsize * *lastrec) + *dataoffset + 1;
  
  
  /* Calculate number of field definitions. */
  
  *numfields=(((unsigned long) *dataoffset)-DBF_BUFFSIZE-1)/DBF_BUFFSIZE; 
  if (*numfields > DBF_MAXFIELDS)
    return DBF_TOO_MANY_FIELDS;
  if (*numfields < 1)
    return DBF_NO_FIELDS;
  
  
  /* Done. */
  
  return DBF_OK;
}
