/* @(#)infoutil.c	1.8 4/13/93 */
/*
 * Notice:  This software is being released into the public
 *          domain.  It may be freely used whole or in part or 
 *          distributed as a whole as long as this notice remains
 *          attached to the source code.  The author and ESRI 
 *          do not assume any liability or responsibility in the 
 *          use or distribution of this software.  Direct any 
 *          questions or comments via email to the author at 
 *          tstellhorn@esri.com.
 *           
 *                             Todd Stellhorn
 *                             ESRI
 */

/***************************************************************************
  Name: infoutil.c 
  
  Description: Contains infolib utility functions.  The following
  functions are contained in this module:
  
  InfoBinarySearch
  InfoSeqSearch
  InfoGetTemplate
  InfoFreeTemplate
  
  History: 12/91 Todd Stellhorn - original coding
  
  10/94 Randy Deardorff - my own peculiar includes.
  
  01/95 Randy Deardorff - added stdio.h.  This is not needed unless
  you are also including rbdpaths.h.
  
  ***************************************************************************/

#include <stdio.h>
#include <rbdpaths.h>
#include INFOLIB_H
#include INFODEFS_H

/***************************************************************************
  Name: InfoBinarySearch
  
  Purpose: This function performs a binary search on the input open
  INFO file based on the item definition and value of str or ival.
  The lowest record number of the matching records and function status
  are returned.  Additionally, the values of IOBuffer and
  CurrentRecord pointed to by ifp are updated.  The function operates
  only on INFO C, I or B item types and the pseudo item $RECNO
  (INFO_CHARACTER_TYPE, INFO_INTEGER_TYPE, INFO_BINARY_TYPE or
  INFO_RECNO_TYPE).  It is assumed that the input INFO file is sorted
  on the item pointed to by idp.  This function will produce undefined
  results if the input file is not sorted on the item pointed to by
  idp.
  
  Parameters:
  ifp ( InfoFile * ) INFO file pointer
  idp ( InfoItemDef * ) INFO item definition
  str ( char * ) Search string
  ival ( long int ) Search integer value
  recno ( long int * ) Record number
  
  Outputs:
  Function - ( short int )  Returned binary search status
  INFO_FAIL == record not found or binary search error
  INFO_SUCCESS == record found
  
  The lowest record number of the matching records is returned via
  recno and the values of IOBuffer and CurrentRecord pointed to by ifp
  are updated.  On non-matching record, the value of recno is set to 0
  and the values of IOBuffer and CurrentRecord are undefined.  On
  error, the value of recno is set to -1 and the values of IOBuffer
  and CurrentRecord are undefined.
  
  History: 12/91 Todd Stellhorn - original coding
  
  02/93 Todd Stellhorn - recno set to 0 on no match, -1 on error
  
  ***************************************************************************/

short int InfoBinarySearch( ifp, idp, str, ival, recno )
     
     InfoFile *ifp;
     InfoItemDef *idp;
     char *str;
     long int ival;
     long int *recno;
     
{
  long int        minrec;
  long int        maxrec;
  long int        currrec;
  short int       cmpcode;
  
  /* set recno to error return value */
  *recno = -1;
  
  if( idp->ItemType == INFO_RECNO_TYPE )
    {
      if( ! InfoReadRecord( ifp, ival ))
        return( INFO_FAIL );
      
      *recno = ival; 
      return( INFO_SUCCESS );
    }
  
  if(( idp->ItemType != INFO_CHARACTER_TYPE ) && 
     ( idp->ItemType != INFO_INTEGER_TYPE ) && 
     ( idp->ItemType != INFO_BINARY_TYPE ))
    return( INFO_FAIL );
  
  currrec = 0;
  minrec = 1;
  maxrec = ifp->NumberRecords;
  
  while( 1 )
    {
      if( minrec > maxrec )
        {
          /* record not found */
          *recno = 0;
          return( INFO_FAIL );
        }
      
      currrec = ( minrec + maxrec ) / 2;
      
      if( ! InfoReadRecord( ifp, currrec ))
        return( INFO_FAIL );
      
      if( ! CompareValues( ifp, idp, str, ival, &cmpcode ))
        return( INFO_FAIL );
      
      if( cmpcode == 0 )
        break;
      
      if( cmpcode > 0 )
        minrec = currrec + 1;
      else
        maxrec = currrec - 1;
    }
  
  while( 1 )
    {
      if( currrec == 1 )
        {
          *recno = currrec;
          return( INFO_SUCCESS );
        }
      
      if( ! InfoReadRecord( ifp, --currrec ))
        return( INFO_FAIL );
      
      if( ! CompareValues( ifp, idp, str, ival, &cmpcode ))
        return( INFO_FAIL );
      
      if( cmpcode != 0 )
        {
          if( ! InfoReadRecord( ifp, ++currrec ))
            return( INFO_FAIL );
          
          *recno = currrec;
          return( INFO_SUCCESS );
        }
    }
}


/***************************************************************************
  Name: InfoSeqSearch
  
  Purpose: This function performs a sequential search on the input
  open INFO file based on the item definition and value of str or
  ival.  The record number of the first matching record and function
  status are returned.  Additionally, the values of IOBuffer and
  CurrentRecord pointed to by ifp are updated.  The function operates
  only on INFO C, I or B item types and the pseudo item $RECNO
  (INFO_CHARACTER_TYPE, INFO_INTEGER_TYPE, INFO_BINARY_TYPE or
  INFO_RECNO_TYPE).  The function searches the input file by starting
  at the top of the file and sequentially searching for the required
  value.
  
  Parameters:
  ifp ( InfoFile * ) INFO file pointer
  idp ( InfoItemDef * ) INFO item definition
  str ( char * ) Search string
  ival ( long int ) Search integer value
  recno ( long int * ) Record number
  
  Outputs:
  Function - ( short int )  Returned sequential search status
  INFO_FAIL == record not found or sequential 
  search error
  INFO_SUCCESS == record found
  
  The lowest record number of the matching records is returned via
  recno and the values of IOBuffer and CurrentRecord pointed to by ifp
  are updated.  On non-matching record, the value of recno is set to 0
  and the values of IOBuffer and CurrentRecord are undefined.  On
  error, the value of recno is set to -1 and the values of IOBuffer
  and CurrentRecord are undefined.
  
  History: 12/91 Todd Stellhorn - original coding
  
  02/93 Todd Stellhorn - recno set to 0 on no match, -1 on error
  
  ***************************************************************************/

short int InfoSeqSearch( ifp, idp, str, ival, recno )
     
     InfoFile *ifp;
     InfoItemDef *idp;
     char *str;
     long int ival;
     long int *recno;
     
{
  long int        currrec;
  short int       cmpcode;
  
  /* set recno to error return value */
  *recno = -1;
  
  if( idp->ItemType == INFO_RECNO_TYPE )
    {
      *recno = ival; 
      return( InfoReadRecord( ifp, ival ));
    }
  
  if(( idp->ItemType != INFO_CHARACTER_TYPE ) && 
     ( idp->ItemType != INFO_INTEGER_TYPE ) && 
     ( idp->ItemType != INFO_BINARY_TYPE ))
    return( INFO_FAIL );
  
  currrec = 1;
  
  while( currrec <= ifp->NumberRecords )
    {
      if( ! InfoReadRecord( ifp, currrec ))
        return( INFO_FAIL );
      
      if( ! CompareValues( ifp, idp, str, ival, &cmpcode ))
        return( INFO_FAIL );
      
      if( cmpcode == 0 )
        {
          *recno = currrec;
          return( INFO_SUCCESS );
        }
      
      currrec++;
    }
  
  /* no match found */
  *recno = 0;
  
  return( INFO_FAIL );
}


/***************************************************************************
  Name: InfoGetTemplate
  
  Purpose: This function returns a pointer to an array of type
  TemplateList based on an open INFO file pointed to by InfoFilePtr.
  The array of type TemplateList contains the item definitions for the
  input INFO file.  Only real items are contained in the array --
  redefined items are excluded.  A pointer to the TemplateList array
  is returned or NULL on error.
  
  Parameters:
  ifp ( InfoFile * ) Input INFO file pointer
  
  Outputs:
  Function - ( TemplateList * ) Returned pointer to template 
  item list or NULL on error
  
  History: 12/91 Todd Stellhorn - original coding
  
  ***************************************************************************/

TemplateList *InfoGetTemplate( ifp )
     
     InfoFile *ifp;
     
{
  short int     i;
  char          buf[ITEMNAME_SIZE];
  NitRecord     nitrec;
  TemplateList *tlp = ( TemplateList * ) NULL;
  FILE         *nitfp = ( FILE * ) NULL;
  
  if( ifp == ( InfoFile * ) NULL )
    return(( TemplateList * ) NULL );
  
  if(( tlp = ( TemplateList * ) malloc( sizeof( TemplateList ) * 
                                       ifp->NumberItems )) == NULL )
    return(( TemplateList * ) NULL );
  
  if(( nitfp = OpenFile( ifp->NitFilePath, InfoREAD )) == ( FILE * ) NULL )
    {
      free( tlp );
      return(( TemplateList * ) NULL );
    }
  
  for( i = 0; i < ifp->NumberItems; i++ )
    {
      if( ! NitReadRecord( nitfp, &nitrec, ( long int ) i ))
        {
          free( tlp );
          CloseFile( nitfp );
          return(( TemplateList * ) NULL );
        }
      
      tlp[i].ItemPosition = nitrec.ItemPosition;
      tlp[i].ItemWidth = nitrec.ItemWidth;
      tlp[i].ItemType = nitrec.ItemType;
      tlp[i].NumberDecimals = nitrec.NumberDecimals;
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


/***************************************************************************
  Name: InfoFreeTemplate
  
  Purpose: This function returns memory allocated to an INFO file
  template to free memory.
  
  Parameters: tlp ( TemplateList * ) Input TemplateList pointer
  
  Outputs: Function - ( short int ) Returned function status
  
  History: 12/92 Todd Stellhorn - original coding
  
  ***************************************************************************/

short int InfoFreeTemplate( tlp )
     
     TemplateList *tlp;
     
{
  free( tlp );
  return( INFO_SUCCESS );
}
