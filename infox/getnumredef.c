/* getnumredef.c
   ------------------------------------------------------------------------
   A function in infoxlib.
   
   Sets the number of redefined items in an info file.
   Returns ok unless error.
   
   Written: 08/15/94 Randy Deardorff.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include INFOLIB_H
#include INFO_H
#include INFODEFS_H

#define TRUE 1
#define FALSE 0
#undef DEBUGGING

short int getnumredef(ifp, numredefitems)
     InfoFile *ifp;
     short int *numredefitems;
{
  short int     i;
  char          buf[ITEMNAME_SIZE];
  NitRecord     nitrec;
  TemplateList *tlp = ( TemplateList * ) NULL;
  FILE         *nitfp = ( FILE * ) NULL;
  
  short int     done, numredefined;
  
  if( ifp == ( InfoFile * ) NULL )
    return INFO_READ_TEMPLATE;
  
  if(( nitfp = OpenFile( ifp->NitFilePath, InfoREAD )) == ( FILE * ) NULL )
    return INFO_READ_TEMPLATE;
  
  for( i = 0; i < ifp->NumberItems; i++ )
    {
      if( ! NitReadRecord( nitfp, &nitrec, ( long int ) i ))
        {
          CloseFile( nitfp );
          return INFO_READ_TEMPLATE;
        }
    }
  
  numredefined = 0;
  done = FALSE;
  while (!(done))
    if( ! NitReadRecord( nitfp, &nitrec, ( long int ) i++ ))
      done = TRUE;
    else
      numredefined++;
  
  if( ! CloseFile( nitfp ))
    return INFO_READ_TEMPLATE;
  
  *numredefitems = numredefined;
  return INFO_SUCCESS;
}
