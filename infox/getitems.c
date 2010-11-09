/* getitems.c
   ------------------------------------------------------------------------
   A function in infoxlib.
   
   Returns a pointer to an array of type TemplateList based on an open
   INFO file pointed to by InfoFilePtr.  The array of type TemplateList
   contains the item definitions for the input INFO file, including any
   redefined items.  A pointer to the TemplateList array is returned or
   NULL on error.
   
   Written: 08/15/94 Randy Deardorff, USEPA, Seattle.
   
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

TemplateList *getitems( ifp, numredefitems )
     InfoFile *ifp;
     short int numredefitems;
{
  short int     i;
  char          buf[ITEMNAME_SIZE];
  NitRecord     nitrec;
  TemplateList *tlp = ( TemplateList * ) NULL;
  FILE         *nitfp = ( FILE * ) NULL;
  
  long int     numitems;
  
  if( ifp == ( InfoFile * ) NULL )
    return(( TemplateList * ) NULL );

  numitems = ifp->NumberItems + numredefitems;
  
  if(( tlp = ( TemplateList * ) malloc( sizeof( TemplateList ) * 
                                       numitems )) == NULL )
    return(( TemplateList * ) NULL );
  
  if(( nitfp = OpenFile( ifp->NitFilePath, InfoREAD )) == ( FILE * ) NULL )
    {
      free( tlp );
      return(( TemplateList * ) NULL );
    }
  
  for( i = 0; i < numitems; i++ )
    {
      if( ! NitReadRecord( nitfp, &nitrec, ( long int ) i ))
        {
          free( tlp );
          CloseFile( nitfp );
          return(( TemplateList * ) NULL );
        }
      tlp[i].ItemPosition = nitrec.ItemPosition;
      tlp[i].NumberDecimals = nitrec.NumberDecimals;
      tlp[i].ItemWidth = nitrec.ItemWidth;
      tlp[i].ItemType = nitrec.ItemType;
      tlp[i].OutputWidth = nitrec.OutputWidth;
      TerminateString( nitrec.ItemName, sizeof( buf ), buf );
      strcpy( tlp[i].ItemName, buf );
    }
  
  if( ! CloseFile( nitfp ))
    {
      free( tlp );
      return(( TemplateList * ) NULL );
    }
  
  return( tlp );
}
