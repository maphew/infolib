/* stritems.c            A function in misclib.
   ------------------------------------------------------------------------
   int stritems(list,dlm)
   char *list;
   int  dlm;
   
   Returns the number of items in list as delimited by dlm.  If dlm is
   space or TAB, then the number of dlms makes no difference.  Otherwise,
   leading dlms are treated as null items, as are consecutive dlms
   between other items.
   
   Written: 07/21/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   07/07/93 Complete rewrite.  It was screwing up sometimes if there were
   extra delimiters.  Also added TAB same as space.
   
   ------------------------------------------------------------------------ */

int stritems(list,dlm)
     char *list;
     int  dlm;
{
  
  int  numitems, ch;
  
  
  /* Default return value. */
  
  numitems=0;
  
  
  /* Set up list and initial numitems based on delimiter. */
  
  if (*list != '\0')
    {
      if (dlm == ' ' || dlm == '\t')
        {
          while (*list != '\0' && *list == dlm)
            *list++;
        }
      else
        numitems=1;
    }
  
  
  /* Scan to the end of the list counting items. */
  
  while (*list != '\0')
    {
      if (dlm==' ' || dlm=='\t')
        
        /* For space or TAB delimited, we count the contiguous chunks of
           characters with one or more delimiter inbetween. */
        
        {
          while (*list != '\0' && *list != dlm)
            *list++;
          while (*list != '\0' && *list == dlm)
            *list++;
          numitems++;
        }
      else
        
        /* For regular delimiters we simply count the delimiters and add to the
           initial number extablished above, ie, 1. */
        
        {
          if (*list++ == dlm)
            numitems++;
        }
    }
  
  
  /* End of list reached (or list is empty). */
  
  return numitems;
}
