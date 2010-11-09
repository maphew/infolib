/* strsubstr.c           A function in misclib.
   ------------------------------------------------------------------------
   int strsubstr(list,item,start,length)
   char *list, *item;
   int  start, length;
   
   Sets item to the substring of list that begins at start and is
   length long.  Returns true if was able to pull the substring.  If
   returning empty string for any reason, returns false.  User must
   make sure item is big enough to contain the substring.
   
   Written: 07/02/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

int strsubstr(list,item,start,length)
     char *list, *item;
     int  start, length;
{
  int i;
  *item='\0';
  if (*list == '\0' || length == 0)
    return 0;
  for (i = 0; i < start; i++)
    {
      *list++;
      if (*list == '\0')
        return 0;
    }
  for (i = start; i < start+length && *list != '\0'; i++)
    {
      *item=*list;
      *list++;
      *item++;
    }
  *item='\0';
  return 1;
}
