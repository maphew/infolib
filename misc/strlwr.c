/* strlwr.c              A function in misclib.
   ------------------------------------------------------------------------
   int strlwr(str)
   char *str;
   
   Converts any uppercase characters in str to lowercase.
   
   Written: 07/04/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

int strlwr(str)
     char *str;
{
  while(*str != '\0')
    {
      if(*str >= 'A' && *str <= 'Z')
        *str=*str+32;
      *str++;
    }
  return 1;
}
