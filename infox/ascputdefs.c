/* ascputdefs.c
   ------------------------------------------------------------------------
   A function in infoxlib.

   Writes the item definitions to an ascii file.
   
   Written: 05/24/93 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   08/24/94 Converted to INFO direct access.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include INFOLIB_H
#include INFO_H
#include INFODEFS_H

int ascputdefs(ioout,itemnums,numitemstodo,dolen,numitems,Template)
     FILE          *ioout;
     short int     *itemnums, numitemstodo, dolen;
     short int     numitems;
     TemplateList  *Template;
{
  long int      i, num;
  unsigned long itemnum;
  short int     itemtype, itemilen, itemolen, itemnumdec;
  char          itemname[INFO_NAMELEN], numdecbuff[3];
  char          *itemtypes="_DCINBFLPVOT";
  
  
  /* Loop over items in order user specified. */
  
  for (i=0; i < numitemstodo; i++)
    {
      
      /* Get index of next item, */
      
      num=itemnums[i];
      
      /* Pull values from arrays. */
      
      strcpy(itemname,Template[num].ItemName);
      itemtype=itemtypes[Template[num].ItemType];
      itemilen=Template[num].ItemWidth;
      itemolen=Template[num].OutputWidth;
      if (itemtype=='D')
        itemolen=dolen;
      itemnumdec=Template[num].NumberDecimals;
      if (itemnumdec < 0)
        strcpy (numdecbuff,"  ");
      else
        sprintf(numdecbuff,"%2d",itemnumdec);
      
      /* Write to out file. */
      
      fprintf(ioout,"%-16s %4d %4d %c %s\n",
              itemname,itemilen,itemolen,itemtype,numdecbuff);
    }
  
  
  /* Done. */
  
  return INFO_OK;
}
