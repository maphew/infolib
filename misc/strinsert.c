/* strinsert.c           A function in misclib.
   ------------------------------------------------------------------------
   int strinsert(str,ch,pos)
   char *str;
   int  ch, pos;
   
   Inserts ch into str at position pos.  Str ends up longer by 1.
   User must make sure str is declared long enough to hold additional
   character.  Ok if str is empty, or pos points to terminating null.
   Thus this can be used to create a string from ch, or concatenate ch
   to str.  Returns true if ok.  Returns false if pos is past end of
   str.
   
   Written: 7/14/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <string.h>

int strinsert(str,ch,pos)
     char *str;
     int  ch, pos;
{
  int  i, len;
  len=strlen(str);
  if (pos >= 0 && pos <= len)
    {
      str=str+len;
      for (i=len; i>pos; i--)
        {
          *(str+1)=*str;
          *str--;
        }
      *(str+1)=*str;
      *str=ch;
      return 1;
    }
  return 0;
}
