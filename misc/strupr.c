/* strupr.c              A function in misclib.
   ------------------------------------------------------------------------
   int strupr(str)
   char *str;
   
   Converts any lowercase characters in str to uppercase.
   
   Written: 07/04/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

int strupr(str)
     char *str;
{
  while(*str != '\0')
    {
      if(*str >= 'a' && *str <= 'z')
        *str=*str-32;
      *str++;
    }
  return 1;
}
