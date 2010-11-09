/* strkeyword.c          A function in misclib.
   ------------------------------------------------------------------------
   int strkeyword(list,item,dlm,searchtype)
   char   *list, *item, *searchtype;
   int    dlm;
   
   Returns the number of the item in list that matches the item
   specified.  If more than one item matches, the search is ambiguous.
   
   Searchtype can be "left" to match from left to right only, or
   "both" to match either direction.  For example "bar" would match
   "foobar" if searchtype was "both", but would not match if
   searchtype was "left".  However, "foo" would match "foobar" either
   way.  Only the first character is of the searchtype argument is
   checked, so it could be just "l" or "b".  Case does not matter.
   
   Return values:
   
   0  No match found.
   -1 Ambuguous.
   n  Matches item n.
   
   Maximum length of items in list is 255.
   
   Written: 09/01/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

#define NOTFOUND 0
#define AMBIGUOUS -1
#define MAXWORDLEN 255
#undef  DEBUGGING

int strkeyword(list,item,dlm,searchtype)
     char   *list, *item, *searchtype;
     int    dlm;
{
  
  char  curitem[MAXWORDLEN+1];
  int   numitems, itemnum, start;
  int   bestnum, bestlen, lenofmatch, itemlen, curitemlen;
  
  
  /* Check for bogus arguments.  Return as if not found if problems. */
  
  numitems=stritems(list,dlm);
  if (numitems < 1)
    return NOTFOUND;
  
  itemlen=strlen(item);
  if (itemlen < 1)
    return NOTFOUND;
  
  strupr(searchtype);
  if (*searchtype != 'L' && *searchtype != 'B')
    return NOTFOUND;
  
  
  /* Arguments pass checks.  Do the job. */
  
#ifdef DEBUGGING
  printf("KEYWORD:   Search mode: \"%s\"\n",searchtype);
#endif
  
  bestlen=0;
  bestnum=0;
  for (itemnum=1; itemnum<=numitems; itemnum++)
    {
      strext(list,itemnum,curitem,dlm);
      curitemlen=strlen(curitem);
      
      /* See to what length item matches curritem according to searchtype.  */
      
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
      
#ifdef DEBUGGING
      printf("    Initial match length: %d\n",lenofmatch);
#endif
      
      /* If match is perfect, return current item number. */
      
      if (lenofmatch == 0)
        return itemnum;
      
      /* If total mismatch, set length of match to zero. */
      
      if (lenofmatch < 0)
        lenofmatch=0;
      
      /* If match less than full length of item, treat as no match. */
      
      if (lenofmatch < itemlen)
        lenofmatch=0;
      
#ifdef DEBUGGING
      printf("    Final   match length: %d\n",lenofmatch);
#endif
      
      /* Check for ambiguity. */
      
      if (lenofmatch > 0 && bestlen > 0 && bestlen != curitemlen)
        if (lenofmatch == bestlen)
          return AMBIGUOUS;
      
      /* If match is better than what we have already, select it as best. */
      
      if (lenofmatch > bestlen)
        {
          if (itemlen <= curitemlen)
            {
              bestnum=itemnum;
              bestlen=lenofmatch;
#ifdef DEBUGGING
              printf("      Best so far.\n");
#endif
            }
        }
    }
  return bestnum;
}
