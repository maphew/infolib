/* dbputdefs.c
   ------------------------------------------------------------------------
   A function in dbflib.

   Writes the field definition section of a dbf file.
   Note byte swapping of integer values.
   
   Written: 09/30/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes by the author unless otherwise noted)
   
   03/31/93 Added check for invalid field types.  Added type F.
      
   07/20/94 Added logic to check for and handle various sizeofs for
   other systems as much as possible.
   
   07/30/94 Moved unions from system header to here.
   
   08/30/94 Loop from zero rather than from 1.

   09/07/94 Added type G fields.
   
   01/21/95 Changed to use a structure for field defs rather than arrays.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H
#include DBFSYS_H

int dbfputdefs(ioout,numfields,tlp)
     FILE          *ioout;
     unsigned long numfields;
     dbfField      *tlp;
{
  unsigned char  byte, iswap[2];
  unsigned long  curbyte, curfield;
  char           fieldname[DBF_NAMELEN], fieldtype, fieldnumdec, ch2[2];
  unsigned short fieldlen;
  union          dbfunion dbfswap;
  union          header head;
  union          fieldrec field;
  
  
  /* Write the field definitions. */
  
  for (curfield=0; curfield < numfields; curfield++)
    {
      
      /* Pull the current field definition from the structure. */

      strcpy(fieldname,tlp[curfield].fieldname);
      fieldtype = tlp[curfield].fieldtype;
      fieldlen = tlp[curfield].fieldlen;
      fieldnumdec = tlp[curfield].fieldnumdec;
      
      /*      printf("  %3ld %-11s %-4c %3d %3d\n", */
      /*        curfield+1,fieldname,fieldtype,fieldlen,fieldnumdec); */
      
      
      /* Make sure field type is valid. */
      
      if (strfind("CNFDLMG",ctoa(fieldtype,ch2),0) < 0)
        {
          printf("Don't know how to handle type \"%c\" fields.\n",fieldtype);
          return DBF_BAD_ITYPE;
        }
      
      
      /* Build the dbf field def io structure. */
      
      memset(field.fieldch,0,DBF_BUFFSIZE); /* Fill with nulls. */
      strcpy(field.fieldst.field_name,fieldname);
      field.fieldst.field_type=fieldtype;
      
      /* Encode length and number of decimal places according to type. */
      
      if (fieldtype == 'N' || fieldtype == 'F')
        {
          field.fieldst.len_info.num_size.len=fieldlen;
          field.fieldst.len_info.num_size.dec=fieldnumdec;
        }
      else
        {
          if (sizeof(unsigned short)==2)
            dbfswap.ushortbuff=fieldlen;
          else
            if (sizeof(unsigned)==2)
              dbfswap.uintbuff=fieldlen;
            else
              return DBF_BAD_SIZEOF;
          field.fieldch[16]=dbfswap.charbuff[0];
          field.fieldch[17]=dbfswap.charbuff[1];
          
#if DOS_SWAP
          iswap[0]=field.fieldch[16];
          iswap[1]=field.fieldch[17];
          field.fieldch[16]=iswap[1];
          field.fieldch[17]=iswap[0];
#endif
          
        }
      
      /* Write the structure to the output file. */
      
      for (curbyte=0; curbyte < sizeof(field.fieldst); curbyte++)
        {
          byte=field.fieldch[curbyte];
          if (fwrite(&byte,sizeof(char),1,ioout) != sizeof(char))
            return DBF_WRITE_DEFS;
        }
    }
  
  
  /* Write the end of field definitions marker. */
  
  byte=DBF_END_OF_DEFS;
  if (fwrite(&byte,sizeof(char),1,ioout) != sizeof(char))
    return DBF_WRITE_DEFS;
  
  
  /* Done. */
  
  return DBF_OK;
}
