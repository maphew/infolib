/* defsdbf2info.c
   ------------------------------------------------------------------------
   A function in infoxlib.

   Converts dBASE field definitions to INFO item definitions.
   
   Written: 10/07/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   08/31/94 Rewrote from defsdbf2e00.  Added specified fields.

   09/07/94 Handle memo fields.
   
   01/21/95 Changed so that this function creates a structure containing the
   item defs.  Returns a pointer to the structure.  Also receives dbf field
   defs as a structure rather than arrays.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H
#include DBFSYS_H
#include INFOLIB_H
#include INFO_H

TemplateList *defsdbf2info(binopt,numfieldstodo,fieldnums,dbftlp,status)
     char          binopt;
     short int     numfieldstodo, *fieldnums;
     dbfField      *dbftlp;
     short int     *status;
{
  TemplateList   *infotlp = ( TemplateList * ) NULL;
  char           fieldname[DBF_NAMELEN], fieldtype, fieldnumdec;
  unsigned short fieldlen;
  short int      itemilen, itemolen, itemnumdec, itemtype, startcol;
  long int       field, fieldnum, totreclen;
  
  
  /* Allocate memory for the info item def structure. */
  
  if ((infotlp = (TemplateList *)malloc(sizeof(TemplateList)*numfieldstodo))
      == NULL)
    {
      *status=INFO_MALLOC_FIELD;
      return(( TemplateList * ) NULL );
    }
  
  
  /* Convert the specified dbf field definitions to INFO item definitions. */
  
  totreclen = 0;
  startcol = 1;
  for (field=0; field < numfieldstodo; field++)
    {
      fieldnum=fieldnums[field];
      strcpy(fieldname,dbftlp[fieldnum].fieldname);
      fieldtype=dbftlp[fieldnum].fieldtype;
      fieldlen=dbftlp[fieldnum].fieldlen;
      fieldnumdec=dbftlp[fieldnum].fieldnumdec;
      totreclen = totreclen + fieldlen;
      if (totreclen > INFO_MAXLEN)
        {
          *status=DBF_TOO_WIDE_FOR_INFO;
          free(infotlp);
          return((TemplateList *) NULL);
        }
      
      /* Convert field types. */
      
      itemilen=fieldlen;
      itemolen=fieldlen;
      itemnumdec=-1;
      switch (fieldtype)
        {
        case 'D' :
          itemtype = INFO_DATE_TYPE;
          break;
        case 'C' :
        case 'L' :
        case 'M' :
        case 'G' :
          itemtype = INFO_CHARACTER_TYPE;
          break;
        case 'N' :
          itemtype = INFO_NUMBER_TYPE;
          if (fieldnumdec == 0)
            if (binopt == 'N')
              itemtype = INFO_INTEGER_TYPE;
            else
              if (fieldlen > 4 && fieldlen < 10)
                {
                  itemtype = INFO_BINARY_TYPE;
                  itemilen=4;
                }
              else
                itemtype = INFO_INTEGER_TYPE;
          else
            {
              itemnumdec=fieldnumdec;
              itemolen=fieldlen;
              if (fieldlen <= 8)
                {
                  itemtype = INFO_NUMBER_TYPE;
                  itemilen=fieldlen;
                }
              else
                {
                  itemtype = INFO_FLOATING_TYPE;
                  if (fieldlen <= 9)
                    itemilen=4;
                  else
                    itemilen=8;
                }
            }
          break;
        case 'F' :
          itemtype = INFO_FLOATING_TYPE;
          if (fieldnumdec == 0)
            if (binopt == 'N')
              itemtype = INFO_INTEGER_TYPE;
            else
              if (fieldlen > 4 && fieldlen < 10)
                {
                  itemtype = INFO_BINARY_TYPE;
                  itemilen=4;
                }
              else
                itemtype = INFO_INTEGER_TYPE;
          else
            {
              itemnumdec=fieldnumdec;
              itemolen=fieldlen;
              itemtype = INFO_FLOATING_TYPE;
              if (fieldlen <= 9)
                itemilen=4;
              else
                itemilen=8;
            }
          break;
        default:
          *status=DBF_BAD_ITYPE;
          free(infotlp);
          return((TemplateList *) NULL);
        }
      
      
      /* Insert the final item def into the item def structure. */
      
      strcpy(infotlp[field].ItemName,fieldname);
      infotlp[field].NumberDecimals = itemnumdec;
      infotlp[field].ItemWidth = itemilen;
      infotlp[field].ItemType = itemtype;
      infotlp[field].OutputWidth = itemolen;
      infotlp[field].ItemPosition = startcol;
      startcol = startcol + itemilen;

      /* Next field. */
      
    }
  
  
  /* Done. */
  
  *status=INFO_OK;
  return(infotlp);
}
