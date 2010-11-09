/* recinfo2asc.c
   ------------------------------------------------------------------------
   A function in infoxlib.

   Converts an INFO data record to ascii fixed width format.
   
   Written: 08/12/94 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   08/15/94 Added redefined items.

   06/09/97 Fixed type I and B conversions to use double with no decimals
   rather than long int.  Was not handling values bigger than 2147483647.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include INFOLIB_H
#include INFO_H

#define TRUE 1
#define FALSE 0
#undef DEBUGGING

int recinfo2asc(ifp,tlp,asciirec,numredefitems,doredefined)
     InfoFile      *ifp;
     TemplateList  *tlp;
     char          *asciirec;
     short int      numredefitems, doredefined;
{
  char        vbuff[4096];
  double      dval;
  char        fmtbuf[128];
  short int   itemnum, i, itemolen;
  long int    numitemstodo;
  
  
  /* Establish the number of items we're interested in. */
  
  if (doredefined && numredefitems > 0)
    numitemstodo=ifp->NumberItems+numredefitems;
  else
    numitemstodo=ifp->NumberItems;
  
  
  /* Decode items record item by item and build output record. */
  
  asciirec[0]='\0';
  for(itemnum = 0; itemnum < numitemstodo; itemnum++)
    {
      if(! InfoDecode(ifp, &tlp[itemnum], vbuff, &dval))
        return INFO_DECODE_RECORD;
      
      itemolen=tlp[itemnum].OutputWidth;
      
      switch(tlp[itemnum].ItemType)
        {
        case INFO_DATE_TYPE         :
          break;
          
        case INFO_CHARACTER_TYPE    :
          break;
          
        case INFO_INTEGER_TYPE      :
          /* OLD sprintf(fmtbuf, "%%%dld", itemolen); */
          /* OLD sprintf(vbuff, fmtbuf, (long int) dval); */
          sprintf(fmtbuf, "%%%d.0lf", itemolen);
          sprintf(vbuff, fmtbuf, dval);
          break;
          
        case INFO_NUMBER_TYPE       : 
          sprintf(fmtbuf, "%%%d.%dlf", itemolen,
                  tlp[itemnum].NumberDecimals);
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
                  tlp[itemnum].NumberDecimals);
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
      
      
      /* Make sure value contains no unprintable characters. */
      
      for (i=0; i < itemolen; i++)
        if (vbuff[i] < ' ' || vbuff[i] > '~')
          vbuff[i]='*';
      
      /* Append current value to output record. */
      
      strcat(asciirec,vbuff);
    }
  
  return INFO_OK;
}
