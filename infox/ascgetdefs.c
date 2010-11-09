/* ascgetdefs.c
   ------------------------------------------------------------------------
   A function in infoxlib.

   Reads the item definitions from an ascii file.
   
   Written: 05/24/93 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   06/03/93 Added dfmt so can check to be sure date item definitions
   are compatible with the selected date format.
   
   5/23/94 Ignore blank lines in the structure file.  Also allow it to be
   delimited by any mixture of tabs or spaces.

   8/26/94 Changes for info direct access.

   11/4/94 Fixed function prototype for sun.
   
   01/24/95 Changed so that this function creates a structure containing the
   item defs.  Returns a pointer to the structure.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include INFOLIB_H
#include INFO_H

#define FALSE   0
#define TRUE    1
#define MAXLEN 82  /* effective max reclen is MAXLEN-2 */

int baditem();

TemplateList *ascgetdefs(ioin,ascnumitems,ascreclen,dfmt,status)
     FILE      *ioin;
     short int *ascnumitems;
     long int  *ascreclen;
     short int dfmt;
     short int *status;
{
  TemplateList *tlp = ( TemplateList * ) NULL;
  short int    i, numelements, itemcount, dlm, tab, space, done, skip;
  short int    itemtype, itemilen, itemolen, itemnumdec;
  char         itemname[INFO_NAMELEN];
  char         irec[MAXLEN+1], temp[40];
  long int     totlen;


  /* Make initial pass through item def file to count items. */
  
  itemcount=0;
  while (fgets(irec, MAXLEN, ioin) != NULL)
    {
      irec[strlen(irec)-1] = 0;
      if (!(strnull(irec)))
        itemcount++;
    }
  rewind(ioin);

    
  /* Make sure we found at least one item. */
  
  if (itemcount < 1)
    {
      baditem(irec);
      *status=INFO_NO_ITEMS;
      return(( TemplateList * ) NULL );
    }
  
  
  /* Allocate memory for the item def structure. */
  
  if ((tlp = (TemplateList *)malloc(sizeof(TemplateList)*itemcount)) == NULL)
    {
      *status=INFO_MALLOC;
      return(( TemplateList * ) NULL );
    }
  
  
  /* Read item definitions until end of file. */
  
  space=' ';
  tab='\t';
  done=FALSE;
  totlen=0;
  itemcount=0;
  while (!(done))
    {
      skip=FALSE;
      if (fgets(irec, MAXLEN, ioin) != NULL)
        {
          irec[strlen(irec)-1] = 0;
          if (!(strnull(irec)))
            {
              dlm=space;
              stredt(irec,tab,space);
              numelements=stritems(irec,dlm);
              if (numelements < 4 || numelements > 5)
                {
                  baditem(irec);
                  *status=INFO_BAD_DEF;
                  return(( TemplateList * ) NULL );
                }
            }
          else
            skip=TRUE;
        }
      else
        done=TRUE;
      
      if (!skip && !done)
        {
          
          /*      printf("%s\n",irec); */
          
          /* Extract values from the input record. */
          
          strext(irec,1,itemname,dlm);
          
          strext(irec,2,temp,dlm);
          itemilen=atoi(temp);
          
          strext(irec,3,temp,dlm);
          itemolen=atoi(temp);
          
          if (numelements == 5)
            {
              strext(irec,5,temp,dlm);
              itemnumdec=atoi(temp);
            }
          else
            itemnumdec=-1;

          strext(irec,4,temp,dlm);
          strupr(temp);
          switch (temp[0])
            {
            case 'D':
              itemtype=INFO_DATE_TYPE;
              itemnumdec=-1;
              itemilen=8;
              if (itemolen != 8 && itemolen != 10)
                {
                  baditem(irec);
                  *status=INFO_BAD_OW_D;
                  return(( TemplateList * ) NULL );
                }
              switch (dfmt)
                {
                case 1:         /* YYYYMMDD */
                case 3:         /* MM/DD/YY */
                  if (itemolen != 8)
                    {
                      baditem(irec);
                      *status=ASCII_DATE_MISMATCH;
                      return(( TemplateList * ) NULL );
                    }
                  break;
                case 2:         /* MM/DD/YYYY */
                  if (itemolen != 10)
                    {
                      baditem(irec);
                      *status=ASCII_DATE_MISMATCH;
                      return(( TemplateList * ) NULL );
                    }
                  break;
                }
              break;
            case 'C':
              itemtype=INFO_CHARACTER_TYPE;
              itemnumdec=-1;
              break;
            case 'I':
              itemtype=INFO_INTEGER_TYPE;
              itemnumdec= -1;
              break;
            case 'B':
              itemtype=INFO_BINARY_TYPE;
              itemnumdec= -1;
              if (itemilen < 2)
                itemilen=2;
              if (itemilen > 2)
                itemilen=4;
              break;
            case 'N':
              itemtype=INFO_NUMBER_TYPE;
              if (itemolen-itemnumdec < 1)
                {
                  baditem(irec);
                  *status=INFO_BAD_NUMDEC;
                  return(( TemplateList * ) NULL );
                }
              break;
            case 'F':
              itemtype=INFO_FLOATING_TYPE;
              if (itemolen-itemnumdec < 1)
                {
                  baditem(irec);
                  *status=INFO_BAD_NUMDEC;
                  return(( TemplateList * ) NULL );
                }
              if (itemilen < 4)
                itemilen=4;
              if (itemilen > 4)
                itemilen=8;
              break;
            default:
              baditem(irec);
              *status=INFO_BAD_ITYPE;
              return(( TemplateList * ) NULL );
            }
          
          
          /* Check for duplicate item names. */
          
          for (i=0; i < itemcount-1; i++)
            if (strcmp(itemname,tlp[i].ItemName) == 0)
              {
                baditem(irec);
                *status=INFO_DUP_NAME;
                return(( TemplateList * ) NULL );
              }
          
          
          /* Check item definintion. */
          
          if (itemilen <= 0)
            {
              baditem(irec);
              *status=INFO_BAD_ILEN;
              return(( TemplateList * ) NULL );
            }
          
          if (itemolen <= 0)
            {
              baditem(irec);
              *status=INFO_BAD_OLEN;
              return(( TemplateList * ) NULL );
            }

          /* Store item definition in the structure. */
          
          strcpy(tlp[itemcount].ItemName,itemname);
          tlp[itemcount].ItemType = itemtype;
          tlp[itemcount].ItemWidth = itemilen;
          tlp[itemcount].OutputWidth = itemolen;
          tlp[itemcount].NumberDecimals = itemnumdec;
          tlp[itemcount].ItemPosition = totlen+1;
          
          /* Increment fixed reclen and and itemcount. */
          
          totlen=totlen+itemolen;
          itemcount++;
        }
    }

  
  /* Make sure record not too long for INFO. */
  
  if (totlen > INFO_MAXLEN)
    {
      *status=INFO_REC_TOO_LONG;
      return(( TemplateList * ) NULL );
    }
  
  
  /* Set return values. */
  
  *ascnumitems=itemcount;
  *ascreclen=totlen;
  
  
  /* Done. */
  
  *status=INFO_OK;
  return(tlp);
}


/* Shows offending item definition. */

int baditem(itemdef)
     char *itemdef;
{
  printf ("Bad item definition: %s\n",itemdef);
  return 1;
}
