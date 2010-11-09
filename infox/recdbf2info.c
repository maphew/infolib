/* recdbf2info.c
   ------------------------------------------------------------------------
   A function in infoxlib.

   Creates an INFO data record from the specified fields in a dbf record.
   
   Written: 10/06/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   03/14/94 Added strip leading zeros from type N and F fields.  This
   allows values such as "000-16000" to be correctly treated as a
   negative number rather than zero.  These are in the Census STF3A
   data.  Note however that dBASE itself treats these as zero.
   
   08/31/94 Rewrote from recdbf2e00.  Added specified fields.
   
   01/21/95 Changed to accept dbf field defs from structure instead of arrays.
   
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H
#include DBFSYS_H
#include INFOLIB_H
#include INFO_H
#include INFODEFS_H
#include INFOSWAP_H

#define TRUE 1
#define FALSE 0
#undef DEBUGGING

int recdbf2info(dbfrec,ifp,infotlp,dbftlp,numfieldstodo,
                fieldnums,fieldstarts,recnum)
     char          *dbfrec;
     InfoFile      *ifp;
     TemplateList  *infotlp;
     dbfField      *dbftlp;     
     short int     numfieldstodo;
     short int     *fieldnums;
     short int     *fieldstarts;
     long int      recnum;
{
  long int        field, fieldnum;
  char            vbuff[INFO_MAXLEN];
  double          dval;
  short int       status;
  unsigned short  fieldlen;
  char            fieldtype;
  
  for(field=0; field < numfieldstodo; field++)
    {
      fieldnum=fieldnums[field];
      
      /* Pull the length and type from the dbf field structure. */
      
      fieldlen=dbftlp[fieldnum].fieldlen;
      fieldtype=dbftlp[fieldnum].fieldtype;
      
      /* Extract the item value to be encoded. */
      
      strncpy(vbuff,&dbfrec[fieldstarts[fieldnum] - 1],fieldlen);
      vbuff[fieldlen] = '\0';

      
      /* Apply filters.
         Note:  It's ok to use strtrim, etc here, since type N and F
         data will always be true strings.  Could not count on this for
         type C data however. */
      
      if (fieldtype == 'N' || fieldtype == 'F')
        {
          strtrim(vbuff,"both",' ');
          strtrim(vbuff,"left",'0');
          if (strnull(vbuff))
            strcpy(vbuff,"0");
        }
      

      /* Encode the item value into the IOBuffer. */
      
      dval = (double) atof(vbuff);
#ifdef DEBUGGING
      /*       printf("Rec %d, item %d=\"%s\"\n",recnum,itemnum,vbuff); */
      if (fieldlen > 9) {
        printf(". vbuff=%s\tdval=%f\n",vbuff,dval);
      }
#endif
      status=InfoEncode(ifp,&infotlp[field],vbuff,dval);
      if (status != INFO_OK)
        {
          printf("Conversion error at field %d in record %d.\n",
                 fieldnum,recnum);
          return INFO_ENCODE_RECORD;
        }
#ifdef DEBUGGING
      if (fieldlen > 9) {
        printf("  vbuff=%s\tdval=%f\n",vbuff,dval);
      }
#endif
      
      
    }
  return DBF_OK;
}
