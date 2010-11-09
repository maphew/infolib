/* strnull.c             A function in misclib.
   ------------------------------------------------------------------------
   int strnull(str)
   char *str;
   
   Returns true if the string is empty or consists of only whitespace
   characters.  Whitespace characters are spaces and tabs.  Otherwise
   returns false.
   
   Written: 07/13/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

int strnull(s)
     char *s;
{
  while (*s != '\0')
    {
      if (*s != ' ' && *s != '\t')
        return 0;
      *s++;
    }
  return 1;
}
