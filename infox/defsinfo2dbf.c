/* defsinfo2dbf.c
   ------------------------------------------------------------------------
   A function in infoxlib.
   
   Creates dBASE field definitions as appropriate for the specified
   INFO item definitions.
   
   Written: 10/05/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   04/05/93 Added the dbver argument, and logic to handle dBASE III v
   dBASE IV field type conversions, etc.
   
   04/06/93 Added check for type F or N with number of decimals set to
   more than the field width minus two.  Info allows field width minus one.
   dBASE chokes if more than fieldwidth minus two.  See defse002dbf.c.

   07/15/94 Added redefined items.  Doredefined is now passed.
   
   08/30/94 Rewrote from defse002dbf.  Added specified items.  Added code
   to resolve duplicate item names rather than bailing out.
   
   01/23/95 Changed so that this function creates a structure containing the
   field defs.  Returns a pointer to the structure.

   04/27/95 Added some type casts to eliminate nuisance compile warnings.
   
   11/21/95 Fixed bug that caused fatal error for N or F fields with a width
   of 1 or 2, and 0 decimals.
   
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

dbfTemplate *defsinfo2dbf(infotlp,numitems,numredefitems,numitemstodo,itemnums,
                          doredefined,dbver,numfields,recsize,status)
     TemplateList   *infotlp;
     short int      numitems, numredefitems, numitemstodo, *itemnums;
     short int      doredefined, dbver;
     unsigned long  *numfields;
     unsigned short *recsize;
     short int      *status;
{
  dbfTemplate    *dbftlp = ( dbfTemplate * ) NULL;
  long int       itemnum, i, ilen, ch, maxitems, item;
  char           itemname[INFO_NAMELEN], tempitemname[INFO_NAMELEN];
  short int      itemolen, itemtype, itemnumdec;
  char           fieldname[DBF_NAMELEN], fieldtype, fieldnumdec, nbuff[4];
  unsigned short fieldlen, totolen;
  
  
  /* Allocate memory for the field def structure. */
  
  if ((dbftlp = (dbfTemplate *)malloc(sizeof(dbfTemplate)*numitemstodo))
      == NULL)
    {
      *status=INFO_MALLOC_DBF;
      return(( dbfTemplate * ) NULL );
    }
  
  
  /* Set maximum allowed number of items. */
  
  switch (dbver)
    {
    case 3 :
      maxitems=DBF3_MAXFIELDS;
      if (numitemstodo > maxitems)
        {
          printf("dBASE III allows only %d items per record.\n",maxitems);
          *status=INFO_TOO_MANY_ITEMS;
          free(dbftlp);
          return((dbfTemplate *) NULL);
        }
      break;
    case 4 :
      maxitems=DBF4_MAXFIELDS;
      if (numitemstodo > maxitems)
        {
          printf("dBASE IV allows only %d items per record.\n",maxitems);
          *status=INFO_TOO_MANY_ITEMS;
          free(dbftlp);
          return((dbfTemplate *) NULL);
        }
      break;
    default:
      *status=INFO_BAD_DBVER;
      free(dbftlp);
      return((dbfTemplate *) NULL);
      break;
    }
  
  
  /* Loop through item definitions creating field definitions from them.
     Note that we initialize totolen to 1 rather than zero since dBASE record
     length is always 1 more than the sum of the field lengths.  This is due
     to the delete flag at column 1. */
  
  totolen=1;
  for (item=0; item < numitemstodo; item++)
    {
      itemnum=itemnums[item];
      strcpy(itemname,infotlp[itemnum].ItemName);
      strcpy(tempitemname,itemname);
      itemolen=infotlp[itemnum].OutputWidth;
      itemtype=infotlp[itemnum].ItemType;
      itemnumdec=infotlp[itemnum].NumberDecimals;
      
      /* Convert item name.
         Here we truncate the name to the maximum allowed by dBASE.
         INFO allows longer names.
         Also we convert all illegal characters in the item name to
         underscores.  INFO allows many characters that dBASE chokes on.
         Also, since we're tweaking the fieldname, it is quite posible that
         we could end up with more than one field with the same name.  Thus
         we have to check for that. */
      
      ilen=strlen(tempitemname);
      if (ilen >= DBF_NAMELEN)
        {
          ilen=DBF_NAMELEN-1;
          tempitemname[DBF_NAMELEN-1]='\0';
        }
      strcpy(fieldname,tempitemname);
      strupr(fieldname);
      
      for (i=0; i < ilen; i++)
        {
          ch=fieldname[i];
          if ( ( (ch < '0' || ch > '9') && (ch < 'A' || ch > 'Z') )
              &&  (ch != '_') )
            fieldname[i]='_';
        }
      
      /* Convert item types, widths, and number of decimals. */
      
      switch (itemtype)
        {
        case INFO_DATE_TYPE :
          fieldtype='D';
          fieldnumdec=0;
          fieldlen=itemolen;   /* Use same as INFO has. */
          fieldlen=8;          /* chop to 8? */
          break;
        case INFO_CHARACTER_TYPE :
          fieldtype='C';
          fieldnumdec=0;
          fieldlen=itemolen;
          break;
        case INFO_INTEGER_TYPE :
          fieldtype='N';
          fieldnumdec=0;
          fieldlen=itemolen;
          break;
        case INFO_NUMBER_TYPE :
          fieldtype='N';
          fieldnumdec=itemnumdec;
          fieldlen=itemolen;
          break;
        case INFO_BINARY_TYPE :
          fieldtype='N';
          fieldnumdec=0;
          fieldlen=itemolen;
          break;
        case INFO_FLOATING_TYPE :
          if (dbver == 3)
            fieldtype='N';
          if (dbver == 4)
            fieldtype='F';
          fieldnumdec=itemnumdec;
          fieldlen=itemolen;
          break;
        default :
          *status=INFO_BAD_ITYPE;
          free(dbftlp);
          return((dbfTemplate *) NULL);
        }
      
      /* Make sure we don't have more decimals than dBASE allows. */
      
      if ((fieldtype == 'F' || fieldtype == 'N') && (((int) fieldnumdec) > 0))
        if (((int) fieldnumdec) > (((int) fieldlen) - 2))
          {
            printf("Item %s has too many decimals.\n",itemname);
            printf("For dbf files, maximum allowed is field width minus 2.\n");
            printf("Use ALTER to change the output width and try again.\n");
            *status=INFO_TOO_MANY_DECS;
            free(dbftlp);
            return((dbfTemplate *) NULL);
          }
      
      /* Place the final values into the field def structure. */
      
      strcpy(dbftlp[item].fieldname, fieldname);
      dbftlp[item].fieldtype = fieldtype;
      dbftlp[item].fieldlen = fieldlen;
      dbftlp[item].fieldnumdec = fieldnumdec;
      
      /* Increment record size. */
      
      totolen=totolen+fieldlen;
      
    }
  
  
  /* Resolve duplicate field names. */
  
  for (item=0; item < numitemstodo; item++)
    for (i=item+1; i < numitemstodo; i++)
      if (strcmp(dbftlp[item].fieldname,dbftlp[i].fieldname) == 0)
        {
          strcpy(itemname,dbftlp[item].fieldname);
          strcpy(fieldname,dbftlp[i].fieldname);
          sprintf(nbuff,"%d",i+1);
          if (strlen(itemname)+strlen(nbuff) < DBF_NAMELEN)
            {
              strcat(itemname,nbuff);
              strcpy(dbftlp[i].fieldname,itemname);
            }
          else
            {
              strsubstr(itemname,fieldname,0, strlen(itemname)-strlen(nbuff));
              strcat(fieldname,nbuff);
              strcpy(dbftlp[i].fieldname,fieldname);
            }
        }
  
  
  /* Set dbf header terms implied by the field defs. */
  
  *numfields=numitemstodo;
  *recsize=totolen;

  
  /* Done. */
  
  *status=INFO_OK;
  return(dbftlp);
}
