/* strunquote.c          A function in misclib.
   ------------------------------------------------------------------------
   int strunquote(str)
   char   *str;
   
   Unquotes str by removing a balanced pair of single quotes.
   Doesn't remove a quote unless it is matched by an opposing quote.
   Always strips leading and trailing spaces and tabs.
   Thus, if not quoted, strips leading and trailing tabs and spaces.
   
   Written: 07/15/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

int strunquote(str)
     char   *str;
{
  int    len;
  if (*str != '\0')
    {
      strtrim(str,"both",' ');
      strtrim(str,"both",'\t');
      len=strlen(str);
      if (len >= 2)
        {
          if (*str=='\'' && *(str + len-1)=='\'')
            {
              *(str + len-1)='\0';
              strlshift(str);
            }
        }
    }
  return 1;
}
