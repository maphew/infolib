/* strcoll.c            A function in misclib.
   ------------------------------------------------------------------------
   int strcollapse(str,badchars)
   char *str, *badchars;
   
   Collapses out specified characters from a string.
   The output string may be shorter, possibly empty.
   
   Written: 08/24/95 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

int strcollapse(str,badchars)
     char *str, *badchars;
     
{
  
  char *strbase, *strcur;
  
  /* Set up. */
  
  strbase=str;
  
  /* Process each bad character one at a time. */
  
  while(*badchars != '\0')
    {
      
      /* Loop through string, collapsing out current bad character. */
      
      while(*str != '\0')
        {
          if (*str==*badchars)
            {
              
              /* Collapse out this bad character. */
              
              strcur=str;
              while(*str != '\0')
                {
                  *str=*(str+1);
                  *str++;
                }
              str=strcur;
            }
          else
            *str++;
        }
      str=strbase;
      *badchars++;
    }
  return 1;
}
