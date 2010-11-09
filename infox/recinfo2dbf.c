/* recinfo2dbf.c
   ------------------------------------------------------------------------
   A function in infoxlib.

   Builds a dbf data record from the specified fields in an INFO data record
   that has been converted to fixed width ascii.
   
   Written: 10/06/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   08/30/94 Rewrote from rece002dbf.  Added specified items.
   
   06/09/97 Fixed type I and B conversions to use double with no decimals
   rather than long int.  Was not handling values bigger than 2147483647.
   
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

#define TRUE 1
#define FALSE 0
#undef DEBUGGING

int recinfo2dbf(ifp,infotlp,dbfrec,itemnums,numitemstodo,rec)
     InfoFile      *ifp;
     TemplateList  *infotlp;
     char          *dbfrec;
     short int     *itemnums, numitemstodo;
     long int      rec;
{
  char        vbuff[4096];
  double      dval;
  char        fmtbuf[128];
  short int   itemnum, num, i, itemolen;
  
  
  /* Decode items record item by item and build output record. */
  
  strcpy(dbfrec," "); /* Always a blank in column zero (deleted flag). */
  for(num = 0; num < numitemstodo; num++)
    {
      itemnum=itemnums[num];
      if(! InfoDecode(ifp, &infotlp[itemnum], vbuff, &dval))
        return INFO_DECODE_RECORD;
      
      itemolen=infotlp[itemnum].OutputWidth ;
      
      switch(infotlp[itemnum].ItemType)
        {
        case INFO_DATE_TYPE         :
          itemolen=8;
          break;
          
        case INFO_CHARACTER_TYPE    :
          break;
          
        case INFO_INTEGER_TYPE      :
          /* OLD sprintf(fmtbuf, "%%%dld", itemolen); */
          /* OLD sprintf(vbuff, fmtbuf, (long int) dval); */
          sprintf(fmtbuf, "%%%d.0lf", itemolen);
          sprintf(vbuff, fmtbuf, dval);
#ifdef DEBUGGING
          if (itemolen > 9) {
            printf("  vbuff=%s\tdval=%f\tfmtbuf=%s\n",vbuff,dval,fmtbuf);
          }
#endif
          break;
          
        case INFO_NUMBER_TYPE       : 
          sprintf(fmtbuf, "%%%d.%dlf", itemolen,
                  infotlp[itemnum].NumberDecimals);
          sprintf(vbuff, fmtbuf, dval);
          break;
          
        case INFO_BINARY_TYPE       :
          /* OLD sprintf(fmtbuf, "%%%dld", itemolen); */
          /* OLD sprintf(vbuff, fmtbuf, (long int) dval); */
          sprintf(fmtbuf, "%%%d.0lf", itemolen);
          sprintf(vbuff, fmtbuf, dval);
          break;
          
        case INFO_FLOATING_TYPE     :
          sprintf(fmtbuf, "%%%d.%dlf", itemolen,
                  infotlp[itemnum].NumberDecimals);
          sprintf(vbuff, fmtbuf, dval);
          break;
          
        case INFO_LEADFILL_TYPE     :
        case INFO_PACKED_TYPE       :
        case INFO_ZEROFILL_TYPE     :
        case INFO_OVERPUNCH_TYPE    :
        case INFO_TRAILINGSIGN_TYPE :
        case INFO_RECNO_TYPE        :
        default                     :
          return INFO_BAD_ITYPE;
        }
      
      
      /* Make sure value is neither too long nor too short. */
      
      vbuff[itemolen]='\0';
      if (strlen(vbuff) < itemolen)
        strpad(vbuff,' ',itemolen);
      
      
      /* Make sure value contains no unprintable characters.
         
         Should never happen with normal info files, but since
         dbf2info permits type C items to contain binary data,
         we have to check for this.  Also, a redefined item could
         contain binary data.
         
         If I remove the strlen and strpad calls above, then remove
         this section completely, then change the strcat below to
         strncat(dbfrec,vbuff,itelolen), it should allow binary data
         to pass through unchanged.  When I try this, however, all
         '\0' are converted to spaces, apparently by InfoDecode.
         Also, some kind of shift happens.  See info2dbf-test a local.
         
         This is something to look into later.
         
         */
      
      for (i=0; i < itemolen; i++)
        if (vbuff[i] < ' ' || vbuff[i] > '~')
          vbuff[i]='*';
      
      /* Append current value to output record. */
      
      strcat(dbfrec,vbuff);
      
      /* Next item. */
      
    }
  
  return INFO_OK;
}
