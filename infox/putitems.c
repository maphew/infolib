/* putitems.c
   ------------------------------------------------------------------------
   A function in infoxlib.
   
   Lists item definitions to the specified output.
   Returns ok unless error.
   
   Written: 08/16/94 Randy Deardorff.
   
   ------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include INFOLIB_H
#include INFO_H
#include INFODEFS_H

#define TRUE 1
#define FALSE 0
#undef DEBUGGING

int putitems(Template, numitemstodo, dostartcols, ioout)
     TemplateList *Template;
     short int    numitemstodo, dostartcols;
     FILE         *ioout;
     
{
  long int    itemnum;
  short int   startcol, itemtype, itemilen, itemolen, itemnumdec;
  char        itemname[INFO_NAMELEN], numdecbuff[3];
  char        *itemtypes="_DCINBFLPVOT";
  
  
  /* List item definitions. */
  
  for (itemnum=0; itemnum < numitemstodo; itemnum++)
    {
      startcol=Template[itemnum].ItemPosition;
      strcpy(itemname,Template[itemnum].ItemName);
      itemtype=itemtypes[Template[itemnum].ItemType];
      itemilen=Template[itemnum].ItemWidth;
      itemolen=Template[itemnum].OutputWidth;
      itemnumdec=Template[itemnum].NumberDecimals;
      
      if (itemnumdec < 0)
        strcpy (numdecbuff,"  ");
      else
        sprintf(numdecbuff,"%2d",itemnumdec);
      
      if (dostartcols)
        fprintf(ioout,"%4d ",startcol);
      fprintf(ioout,"%-16s %4d %4d %c %s\n",
              itemname,itemilen,itemolen,itemtype,numdecbuff);
    }
  return INFO_OK;
}
