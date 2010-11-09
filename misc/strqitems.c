/* strqitems.c           A function in misclib.
   ------------------------------------------------------------------------
   int strqitems(list,itemdlm,textdlm)
   char *list;
   int  itemdlm, textdlm;
   
   Returns the number of items in list as delimited by itemdlm, with
   optional text delimiters enclosing items.
   
   If an item is enclosed within text delimiters, then the item may
   contain the item delimiter,  eg "Kelso, WA".
   
   If the item delimiter space or TAB, then the number of item delimiters
   makes no difference.  Otherwise, leading and trailing item delimiters
   are treated as null items, as are consecutive item delimiters between
   other items.
   
   Written: 07/07/93 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#define TRUE 1
#define FALSE 0

int strqitems(list,itemdlm,textdlm)
     char *list;
     int  itemdlm, textdlm;
{
  
  int  numitems, ch, ignoreitemdlms;
  
  
  /* Default return value. */
  
  numitems=0;
  ignoreitemdlms=FALSE;
  
  
  /* Set up list and initial numitems based on the item delimiter. */
  
  if (*list != '\0')
    {
      if (itemdlm == ' ' || itemdlm == '\t')
        {
          while (*list != '\0' && *list == itemdlm)
            *list++;
        }
      else
        numitems=1;
    }
  
  
  /* Scan to the end of the list counting items. */
  
  while (*list != '\0')
    {
      
      
      /* Set flag indicating whether we are ignoring item delimiters. */
      
      if (*list == textdlm)
        ignoreitemdlms=(!(ignoreitemdlms));
      
      
      /* For space or TAB delimited, we count the contiguous chunks of
         characters with one or more item delimiter in between.
         If the chunk of characters we're looking at begins with a text
         delimiter, then we ignore any item delimiters until we get to
         the matching text delimiter. */
      
      if (itemdlm==' ' || itemdlm=='\t')
        {
          if (!(ignoreitemdlms))
            {
              while (*list != '\0' && *list != itemdlm)
                *list++;
            }
          else
            {
              *list++;
              
              while (*list != '\0' && *list != textdlm)
                *list++;
              
              if (*list == textdlm)
                ignoreitemdlms=FALSE;
              
              *list++;
            }
          while (*list != '\0' && *list == itemdlm)
            *list++;
          numitems++;
        }
      
      
      /* For regular item delimiters we simply count the item delimiters that
         occur outside of quoted items, adding to the initial number
         extablished above, ie, 1. */
      
      else
        {
          if (*list++ == itemdlm && (!(ignoreitemdlms)))
            numitems++;
        }
    }
  
  
  /* End of list reached (or list is empty). */
  
  return numitems;
}
