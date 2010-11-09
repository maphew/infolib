/* strunquote.c          A function in misclib.
   ------------------------------------------------------------------------
   int strqunquote(str,quotechar)
   char   *str;
   int    quotechar;
   
   Unquotes str by removing a balanced pair of the specified quote
   characters.  Doesn't remove a quote character unless it is matched by
   an opposing quote character.  Always strips leading and trailing
   spaces and tabs.  Thus, if not quoted, only action is to strip leading
   and trailing tabs and spaces.
   
   Returns TRUE if the string was successfully unquoted, or if it was not
   quoted to begin with.  Returns FALSE if the quotes are not balanced.
   
   Written: 07/08/93 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

int strqunquote(str,quotechar)
     char   *str;
     int    quotechar;
{
  int    len, retval;
  
  retval=1;
  
  if (*str != '\0')
    {
      strtrim(str,"both",' ');
      strtrim(str,"both",'\t');
      len=strlen(str);
      if (len >= 2)
        {
          if (*str==quotechar)
            {
              if (*(str + len-1)==quotechar)
                {
                  *(str + len-1)='\0';
                  strlshift(str);
                }
              else
                {
                  retval=0;
                }
            }
          else
            {
              if (*(str + len-1)==quotechar)
                {
                  retval=0;
                }
            }
        }
    }
  return retval;
}
