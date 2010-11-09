/* dbfgetdefs.c
   ------------------------------------------------------------------------
   A function in dbflib.

   Reads the field definition section of a dbf file.
   Places the field definitions into the arrays passed as arguments.
   Note byte swapping of integer values.
   
   Written: 09/29/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes by the author unless otherwise noted)
   
   03/31/93 Added check for invalid field types.  Added type F.
   
   04/01/93 Added check to make sure we read to dataoffset.
   Apparently some dbf files have dataoffsets other than
   (numfields*32)+33, with orphaned bytes between the end of field
   definitions marker and the actual start of the data.  Thus we need
   to check for and skip over these garbage bytes!  Required adding
   parameter dataoffset so all callers will need attention.
   
   07/20/94 Added logic to check for and handle various sizeofs for
   other systems as much as possible.
   
   07/30/94 Moved unions from system header to here.
   
   08/30/94 Loop from zero rather than from 1.

   09/07/94 Added type G fields.
   
   01/21/95 Changed so that this function creates a structure containing the
   field defs.  Returns a pointer to the structure.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H
#include DBFSYS_H

#undef DEBUGGING

dbfField *dbfgetdefs(ioin,numfields,dataoffset,status)
     FILE            *ioin;
     unsigned long   numfields;
     unsigned short  dataoffset;
     short int       *status;
{
  dbfField       *tlp = ( dbfField * ) NULL;
  unsigned char  byte, iswap[2];
  unsigned long  index, numbytes;
  char           fieldname[DBF_NAMELEN], fieldtype, fieldnumdec, ch2[2];
  unsigned long  fieldlen;
  unsigned short doff;
  union          dbfunion dbfswap;
  union          header head;
  union          fieldrec field;
  
  
  /* Allocate memory for the field def structure. */
  
  if ((tlp = (dbfField *)malloc(sizeof(dbfField)*numfields)) == NULL)
    {
      *status=DBF_MALLOC_FIELD;
      return(( dbfField * ) NULL );
    }
  
  
  /* Read the field definitions. */
  
  for (index=0; index < numfields; index++)
    {
      
      /* Read the next field def in to the field def structure. */
      
      for (numbytes=0; numbytes < sizeof(field.fieldst); numbytes++)
        if (fread(&byte,sizeof(char),1,ioin) == sizeof(char))
          field.fieldch[numbytes]=byte;
        else
          {
            *status=DBF_READ_DEFS;
            free(tlp);
            return((dbfField *) NULL);
          }  
      
      if (numbytes != sizeof(field.fieldst))
        {
          *status=DBF_READ_DEFS;
          free(tlp);
          return((dbfField *) NULL);
        }  
      
      
      /* Extract components. */
      
      strcpy(fieldname,field.fieldst.field_name);
      fieldtype=field.fieldst.field_type;
      
      /* Make sure field type is valid. */
      
      if (strfind("CNFDLMG",ctoa(fieldtype,ch2),0) < 0)
        {
          printf("Don't know how to handle type \"%c\" fields.\n",fieldtype);
          *status=DBF_BAD_ITYPE;
          free(tlp);
          return((dbfField *) NULL);
        }
      
      /* Decode length and number of decimal places according to type. */
      
      if (fieldtype == 'N' || fieldtype == 'F')
        {
          fieldlen=field.fieldst.len_info.num_size.len;
          fieldnumdec=field.fieldst.len_info.num_size.dec;
        }
      else
        {
          
#if DOS_SWAP
          iswap[0]=field.fieldch[16];
          iswap[1]=field.fieldch[17];
          field.fieldch[16]=iswap[1];
          field.fieldch[17]=iswap[0];
#endif
          
          dbfswap.charbuff[0]=field.fieldch[16];
          dbfswap.charbuff[1]=field.fieldch[17];
          if (sizeof(unsigned short)==2)
            fieldlen=dbfswap.ushortbuff;
          else
            if (sizeof(unsigned)==2)
              fieldlen=dbfswap.uintbuff;
            else
              {
                *status=DBF_BAD_SIZEOF;
                free(tlp);
                return((dbfField *) NULL);
              }  
          fieldnumdec=0;
        }
      
      /* Place the final values into the field def structure. */
      
#ifdef DEBUGGING
      printf("dbfgetdefs: %3ld %-11s %-4c %3u %3d\n",
             index+1,fieldname,fieldtype,fieldlen,fieldnumdec);
#endif
      
      strcpy(tlp[index].fieldname, fieldname);
      tlp[index].fieldtype = fieldtype;
      tlp[index].fieldlen = fieldlen;
      tlp[index].fieldnumdec = fieldnumdec;
      
    }
  
  
  /* Skip over the end of field definitions marker. */
  
  if (fread(&byte,sizeof(char),1,ioin) != sizeof(char))
    {
      *status=DBF_READ_DEFS;
      free(tlp);
      return((dbfField *) NULL);
    }  
  
  
  /* Make sure the end of field definitions marker was there. */
  
  if (byte != DBF_END_OF_DEFS)
    {
      *status=DBF_BAD_EODEFS;
      free(tlp);
      return((dbfField *) NULL);
    }  
  
  
  /* Make sure stored dataoffset is less than what is should be. */
  
  doff=(numfields * DBF_BUFFSIZE)+DBF_BUFFSIZE+1;
  if (dataoffset < doff)
    {
      printf("Data offset too small for the number of fields defined.");
      printf("Will dBASE load it?  If so, please report this as a bug.");
      *status=DBF_BAD_OFFSET;
      free(tlp);
      return((dbfField *) NULL);
    }
  
  
  /* If stored dataoffset is more than what it should be, skip over garbage
     bytes to true beginning of data section.  Yes, this really happens! */
  
  while (doff++ < dataoffset)
    if (fread(&byte,sizeof(char),1,ioin) != sizeof(char))
      {
        *status=DBF_READ_DEFS;
        free(tlp);
        return((dbfField *) NULL);
      }
  
  
  /* Done. */
  
  *status=DBF_OK;
  return(tlp);
}
