/* strlakw.c             A function in misclib.
   ------------------------------------------------------------------------
   int strlakw(list,item,dlm,searchtype,numperline,message)
   char   *list, *item, *searchtype, *message;
   int    dlm, numperline;
   
   List ambiguous keywords.  Lists on the screen all items in list
   that are possible matches with item.  Useful after calling
   strkeyword and determining that the item is ambiguous to list the
   possible matches to the screen.
   
   Items are listed numperline per line with tabs inbetween.
   Maximum item length is 255 characters.
   Uses fmod, thus requires math.h.
   
   Prints item value in quotes, followed by message.  For example, if
   item were "foo" and message were "is ambiguous, possible matches:"
   then the following line would be printed: <"foo" is ambiguous,
   possible matches:>
   
   Searchtype can be "left" to match from left to right only, or
   "both" to match either direction.  For example "bar" would match
   "foobar" if searchtype was "both", but would not match if
   searchtype was "left".  However, "foo" would match "foobar" either
   way.  Only the first character is of the searchtype argument is
   checked, so it could be just "l" or "b".  Case does not matter.
   
   Returns the number of items in list that are possible matches with
   item.
   
   Written: 09/04/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <rbdpaths.h>
#include MISC_H

#define MAXWORDLEN 255
#undef  DEBUGGING

int strlakw(list,item,dlm,searchtype,numperline,message)
     char   *list, *item, *searchtype, *message;
     int    dlm, numperline;
{
  
  char  curitem[MAXWORDLEN+1];
  int   numitems, itemnum, start;
  int   lenofmatch, itemlen, curitemlen;
  int   nummatches;
  double  dmunmatches, dnumperline;
  
  dnumperline=numperline;
  nummatches=0;
  
  
  /* Check for bogus arguments.  Bail out if problems. */
  
  
  numitems=stritems(list,dlm);
  if (numitems < 1)
    return nummatches;
  
  itemlen=strlen(item);
  if (itemlen < 1)
    return nummatches;
  
  strupr(searchtype);
  if (*searchtype != 'L' && *searchtype != 'B')
    return nummatches;
  
  
  /* Arguments pass checks.  Do the job. */
  
  printf("\"%s\" %s\n",item,message);
  
  for (itemnum=1; itemnum<=numitems; itemnum++)
    {
      strext(list,itemnum,curitem,dlm);
      curitemlen=strlen(curitem);
      
      /* See to what length item matches curitem according to searchtype.  */
      
#ifdef DEBUGGING
      printf("  Checking \"%s\"\n",curitem);
#endif
      
      /* Extrapolate left to right. */
      
      if (*searchtype == 'L')
        {
          lenofmatch=stricom(curitem,item);
        }
      
      /* Extrapolate both directions. */
      
      if (*searchtype == 'B')
        {
          start=strfind(curitem,item,0);
          if (start < 0)
            lenofmatch=-1;
          if (start == 0)
            lenofmatch=stricom(curitem,item);
          if (start > 0)
            lenofmatch=itemlen;
        }
      
      
      /* If match less than full length of item, treat as no match. */
      
      if (lenofmatch < itemlen)
        lenofmatch=0;
      
      
      /* If matches at all, list current item. */
      
      if (lenofmatch > 0)
        {
          nummatches++;
          dmunmatches=nummatches;
          
          printf("%s",curitem);
          
          if (fmod(dmunmatches,dnumperline) == 0)
            putchar('\n');
          else
            {
              putchar('\t');
              if (curitemlen < 8)
                putchar('\t');
            }
        }
    }
  if (nummatches > 0)
    {
      if (fmod(dmunmatches,dnumperline) != 0)
        putchar('\n');
    }
  return nummatches;
}
