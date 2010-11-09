/* strtype.c             A function in misclib.
   ------------------------------------------------------------------------
   int strtype(str)
   char *str;
   
   Checks str and returns an integer code based on the string type.
   
   2  - Has at least one control code.
   1  - A character string with no control codes.
   0  - An empty string.
   -1 - An integer.
   -2 - A real value.
   -3 - An E format value: 2.0E5, 1.45E-4, etc.
   
   Written: 07/05/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

int strtype(str)
     char *str;
{
  int  pos, c;
  int  numdots, numminuses, numpluses, numes, numctrl, numother, numnum;
  int  retctrl, retalpha, retint, retreal, rete;
  
  numdots=0;
  numminuses=0;
  numctrl=0;
  numother=0;
  numpluses=0;
  numes=0;
  numnum=0;
  
  retctrl=2;
  retalpha=1;
  retint=-1;
  retreal=-2;
  rete=-3;
  
  if (*str == '\0')
    return 0;
  
  pos=0;
  while(*str != '\0')
    {
      c=*str;
      
      if (c < ' ' || c > '~')
        numctrl++;
      
      if (c >= '0' && c <= '9')
        numnum++;
      else
        if (c != '.' && c != '-' && c != '+' && c != 'e' && c != 'E')
          numother++;
      
      if (c == '.')
        {
          numdots++;
          if (numdots > 1)
            numother++;
        }
      
      if (c == 'e' || c == 'E')
        {
          numes++;
          if (numes > 1 || numnum == 0)
            numother++;
        }
      
      if (c == '-')
        {
          numminuses++;
          if (numes == 0)
            {
              if (numminuses > 1 || pos > 0)
                numother++;
            }
          if (numes == 1)
            {
              if (numminuses > 2)
                numother++;
            }
        }
      
      if (c == '+')
        {
          numpluses++;
          if (numes == 0)
            {
              if (numpluses > 1 || pos > 0)
                numother++;
            }
          if (numes == 1)
            {
              if (numpluses > 2)
                numother++;
            }
        }
      *str++;
      pos++;
    }
  if (numctrl>0)
    return retctrl;
  if (numother>0)
    return retalpha;
  if (numes == 1)
    return rete;
  if (numdots == 1)
    return retreal;
  return retint;
}
