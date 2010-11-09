/* strext.c              A function in misclib.
   ------------------------------------------------------------------------
   int strext(list,numtoget,item,dlm)
   char *list,*item;
   int  numtoget,dlm;
   
   This funtion sets item to the numtogetth element in list as
   delimited by dlm.  If dlm is space or TAB, then the number of
   delimiters makes no difference.  If dlm is not space or TAB, then
   leading delimiters are treated as null items, as are consecutive
   delimiters between other items.
   
   Returns TRUE if was able to extract item.  If returning empty item,
   the return value is FALSE.
   
   Written: 06/26/92 Randy Deardorff.  USEPA Seattle.

   Modification log: (all changes are by the author unless otherwise noted)
   
   06/01/93 Changed so that default return is empty item, and
   extraction happens only if list is not null and numtoget is reater
   than or equal to 1.  I did this because in some cases, if there
   were leading delimiters, and item one was to be extracted, the
   return item seemed to point to some other unrelated area of memory
   (like maybe the next declared variable?).  Not sure why, but the
   change fixed it.
   
   07/02/93 Complete rewrite.  Previous version had trouble with
   consecutive delimiters between item values.
   
   ------------------------------------------------------------------------ */

int strext(list,numtoget,item,dlm)
     char *list,*item;
     int  numtoget,dlm;
{
  char *itemstart;
  int  curitemnum, ch;
  
  itemstart=item;
  curitemnum=1;
  
  if (numtoget > 0)
    {
      while (*list != '\0' && curitemnum <= numtoget)
        {
          if (dlm==' ' || dlm=='\t')
            
            /* Space or TAB delimited. */
            
            {
              
              /* Skip over delimiters to next non-delimiter. */
              
              while (*list != '\0' && *list == dlm)
                *list++;
              
              /* Copy characters to item to next delimiter. */
              
              while (*list != '\0' && *list != dlm)
                *item++=*list++;
              
              /* If this is not the item we wanted, rewind item to start. */
              
              if (curitemnum != numtoget)
                item=itemstart;
              
              /* Next item. */
              
              curitemnum++;
              
            }
          
          else
            
            /* Not space or TAB delimited. */
            
            {
              ch=*list++;
              if (ch == dlm)
                curitemnum++;
              else
                if (curitemnum==numtoget)
                  *item++=ch;
            }
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
