/* strlshift.c           A function in misclib.
   ------------------------------------------------------------------------
   int strlshift(str)
   char *str;
   
   Does a delete and suck right.  The character at the pointer is
   deleted, and those to the right of it are pulled leftward to fill
   the gap.  The string ends up shorter by one, possibly empty.
   
   Written: 07/10/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

int strlshift(str)
     char *str;
{
  while (*str != '\0')
    {
      *str=*(str+1);
      *str++;
    }
  return 1;
}
