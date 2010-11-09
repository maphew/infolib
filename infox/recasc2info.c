/* recasc2info.c
   ------------------------------------------------------------------------
   A function in infoxlib.

   Builds an INFO internal format data record from the specified fields
   in an ascii data record.  The INFO record is placed in the IOBuffer
   associated with the INFO file.
   
   Written: 08/17/94 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   09/29/94 Allowed for other than double quote as quote character.
   
   01/24/95 Elimiated todo arrays and passed ascii item template instead.

   09/06/95 Elimiated error checking on imcoming date fields.  Just take
   what is given.  This allows empty dates.  INFO allows this.
   
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include INFOLIB_H
#include INFO_H
#include INFODEFS_H
#include INFOSWAP_H

#define TRUE 1
#define FALSE 0
#undef DEBUGGING

int ascbadvalue();

int recasc2info(ascrec,ifp,infotlp,numitems,nums,
                asciitlp,ffmt,dfmt,dlm,quotechar,recnum)
     char         *ascrec;
     InfoFile     *ifp;
     TemplateList *infotlp, *asciitlp;
     short int    *nums;
     short int    numitems, ffmt, dfmt, dlm, quotechar;
     long int     recnum;
{
  char            vbuff[INFO_MAXLEN];
  double          dval;
  short int       status, itemnum;
  char            yyyy[5], cc[3], yy[3], mm[3], dd[3];
  short int       c, m, y, d;
  
  /* For each item from the template. */
  
  for(itemnum = 0; itemnum < numitems; itemnum++)
    {
      
      /* Extract the item value to be encoded. */
      
      switch (ffmt)
        {
          
        case 1 :         /* FIXED */
          strncpy(vbuff,&ascrec[asciitlp[nums[itemnum]].ItemPosition - 1],
                  asciitlp[nums[itemnum]].OutputWidth);
          vbuff[asciitlp[nums[itemnum]].OutputWidth] = '\0';
          break;
          
        case 2 :         /* LIST */
          if (quotechar > 0)
            {
              strqext(ascrec,nums[itemnum]+1,vbuff,' ',quotechar);
              if (!strqunquote(vbuff,quotechar))
                return ASCII_BAD_QUOTE;
            }
          else
            strext(ascrec,nums[itemnum]+1,vbuff,' ');
          break;
          
        case 100 :       /* USER-SPECIFIED */
          if (quotechar > 0)
            {
              strqext(ascrec,nums[itemnum]+1,vbuff,dlm,quotechar);
              if (!strqunquote(vbuff,quotechar))
                return ASCII_BAD_QUOTE;
            }
          else
            strext(ascrec,nums[itemnum]+1,vbuff,dlm);
          break;
          
        default :
          return ASCII_BAD_IFMT;
          break;
        }
      
      
      /* Handle date fields. */
      
      if (asciitlp[nums[itemnum]].ItemType == INFO_DATE_TYPE)
        {
          strtrim(vbuff,"both",' ');
          switch (dfmt)
            {
            case 1:         /* YYYYMMDD */
              if (strnull(vbuff))
                strcpy(vbuff,"        ");
              if (strlen(vbuff) != 8)
                {
                  ascbadvalue(recnum,itemnum,vbuff);
                  return INFO_BAD_DATE;
                }
              strsubstr(vbuff,cc,0,2);
              strsubstr(vbuff,yy,2,2);
              strsubstr(vbuff,mm,4,2);
              strsubstr(vbuff,dd,6,2);
              break;
              
            case 2:         /* MM/DD/YYYY */
              if (strnull(vbuff))
                strcpy(vbuff,"          ");
              if (strlen(vbuff) != 10)
                {
                  ascbadvalue(recnum,itemnum,vbuff);
                  return INFO_BAD_DATE;
                }
              strsubstr(vbuff,cc,6,2);
              strsubstr(vbuff,yy,8,2);
              strsubstr(vbuff,mm,0,2);
              strsubstr(vbuff,dd,3,2);
              d=atoi(dd);
              break;
              
            case 3:         /* MM/DD/YY */
              if (strnull(vbuff))
                strcpy(vbuff,"        ");
              if (strlen(vbuff) != 8)
                {
                  ascbadvalue(recnum,itemnum,vbuff);
                  return INFO_BAD_DATE;
                }
              sprintf(cc,"%2i",INFO_CENTURY / 100);
              strsubstr(vbuff,yy,6,2);
              strsubstr(vbuff,mm,0,2);
              strsubstr(vbuff,dd,3,2);
              break;
            }
          sprintf(vbuff,"%s%s%s%s",cc,yy,mm,dd);
        }
      
      
      /* Encode the item value into the IOBuffer. */
      
      dval = (double) atof(vbuff);
#ifdef DEBUGGING
      printf("Rec %d, item %d=\"%s\"\n",recnum,itemnum,vbuff);
#endif
      status=InfoEncode(ifp,&infotlp[itemnum],vbuff,dval);
      if (status != INFO_OK)
        {
          printf("Conversion error at field %d in record %d.\n",
                 nums[itemnum]+1,recnum);
          return INFO_ENCODE_RECORD;
        }
    }
  return INFO_OK;
}


/* Provide user with details about a bad value in an ascii record. */

int ascbadvalue(rec,itemnum,value)
     long int      rec;
     short int     itemnum;
     char          *value;
{
  printf("Unacceptable value for field %d in record %d.\n",itemnum+1,rec);
  printf("The offending value is: \"%s\"\n",value);
  return 1;
}
