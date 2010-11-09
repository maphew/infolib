/* stredt.c              A function in misclib.
   ------------------------------------------------------------------------
   int stredt(str,inchar,outchar)
   char *str;
   int  inchar,outchar;
   
   Replaces all occurrences of inchar in str with outchar.
   
   Written: 07/01/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

int stredt(str,inchar,outchar)
     char *str;
     int  inchar,outchar;
{
  while (*str != '\0')
    {
      if (*str == inchar)
        *str=outchar;
      *str++;
    }
  return 1;
}
