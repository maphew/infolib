/* dbfgetrec.c
   ------------------------------------------------------------------------
   A function in dbflib.

   Reads a data record as a string from the dbf file.  Appends the
   null to make the record a true string.  Note however, that the
   record cannot be counted on to in fact be a true string unless
   upopt is 'C'.  End of file while reading is a fatal error.
   
   Written: 09/29/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes by the author unless otherwise noted)
   
   92/10/16 Added check for non-printable characters in the data
   record.  There should never be any in dBASE III+.  ESAT had dbf
   files with nulls in the data.  These were created from Lotus.
   
   93/03/25 Added option for what to do if non-printable characters
   are found.  'F' means to fail.  'W' means to give a warning.  'C'
   means to convert to '*'.  Anything else means ignore them.

   94/09/13 Changed unprintable marker from '?' to '*'.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H

#define TRUE 1
#define FALSE 0

int dbfgetrec(ioin,irec,recsize,upopt)
     
     FILE           *ioin;
     char           *irec;
     unsigned short recsize;
     char           upopt;
     
{
  unsigned char  byte;
  unsigned long  numbytes;
  
  
  /* Read the data record into the string. */
  
  for (numbytes=0; numbytes < recsize; numbytes++)
    if (fread(&byte,sizeof(char),1,ioin) == sizeof(char))
      {
        if (byte < ' ' || byte > '~')
          {
            if (upopt=='F')
              return DBF_UNPRINT_DATA;
            if (upopt=='W')
              printf("Warning: Non-printable character in data.\n");
            if (upopt=='C')
              byte='*';
          }
        *irec++=byte;
      }
    else
      return DBF_READ_DATA;
  
  if (numbytes != recsize)
    return DBF_READ_DATA;
  
  
  /* Append a null to make the return value a true string. */
  
  *irec='\0';
  
  /* Done. */
  
  return DBF_OK;
}
