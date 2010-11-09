/* strqext.c             A function in misclib.
   ------------------------------------------------------------------------
   int strqext(list,numtoget,item,itemdlm,textdlm)
   char *list,*item;
   int  numtoget,itemdlm,textdlm;
   
   This funtion sets item to the numtogetth element in list as delimited
   by itemdlm.  If itemdlm is space or TAB, then the number of item
   delimiters makes no difference.  Otherwise, leading and trailing item
   delimiters are treated as null items, as are consecutive item
   delimiters between other items.
   
   Any item may be optionally enclosed in text delimiters.  In this case,
   item delimiters that occur between text delimiters are ignored.  This
   allows extraction of items from lists such as: "Kelso, WA",98478
   
   Returns TRUE if was able to extract item.  If returning empty item,
   the return value is FALSE.
   
   Written: 07/06/93 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#define TRUE 1
#define FALSE 0

int strqext(list,numtoget,item,itemdlm,textdlm)
     char *list,*item;
     int  numtoget,itemdlm,textdlm;
     
{
  char *itemstart;
  int  curitemnum, ch, previousch, ignoreitemdlms;
  
  itemstart=item;
  curitemnum=1;
  ignoreitemdlms=FALSE;
  ch=1;
  previousch=itemdlm+1;
  
  if (numtoget > 0)
    {
      
      /* For whitespace delimited, skip leading delimiters. */
      
      if (itemdlm == ' ' || itemdlm == '\t')
        while (*list != '\0' && *list == itemdlm)
          *list++;
      
      
      while (ch != '\0' && curitemnum <= numtoget)
        {
          ch=*list++;
          
          if (ch == textdlm)
            ignoreitemdlms=(!(ignoreitemdlms));
          
          if (ch == itemdlm)
            {
              if (ignoreitemdlms)
                {
                  if (curitemnum == numtoget)
                    {
                      *item++=ch;
                    }
                }
              else
                {
                  if (itemdlm==' ' || itemdlm=='\t')
                    {
                      if (previousch != itemdlm)
                        curitemnum++;
                    }
                  else
                    {
                      curitemnum++;
                    }
                }
            }
          else
            {
              if (curitemnum == numtoget)
                {
                  *item++=ch;
                }
            }
          previousch=ch;
        }
    }
  
  
  /* Add null to end of item we've extracted.
     Or, if we haven't extracted anything, return empty string. */
  
  *item='\0';
  
  
  /* Done. */
  
  if (*itemstart=='\0')
    return 0;
  else
    return 1;
}
