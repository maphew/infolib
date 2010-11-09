/* @(#)infonit.c	1.9 2/26/94 16:50:33 */
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
  Name: infonit.c
  
  Description: Contains infolib NIT file functions.  The following
  functions are contained in this module:
  
  NitGetRecord
  NitReadRecord
  NitWriteRecord
  
  History: 12/91  Todd Stellhorn  - original coding
  
  10/94  Randy Deardorff - my own peculiar includes.
  
  01/95 Randy Deardorff - added stdio.h.  This is not needed unless
  you are also including rbdpaths.h.
  
  ***************************************************************************/

#include <stdio.h>
#include <rbdpaths.h>
#include INFOLIB_H
#include INFODEFS_H
#include INFOSWAP_H


/***************************************************************************
  Name: NitGetRecord
  
  Purpose: This function searches the input NIT file for a record
  matching the input INFO item name.  The matched record and its file
  position are returned.
  
  Parameters:
  nitfile ( char * ) NIT file path
  itemname ( char * ) INFO item name
  retrec ( NitRecord * ) NIT record
  pos ( long int * ) Record position
  
  Outputs:
  Function - ( short int ) Returned function status
  INFO_FAIL == error returning record
  INFO_SUCCESS == matching record found
  
  On success, the matching NIT record and NIT file position are
  returned.  On error, the values for retrec and pos are undefined.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int NitGetRecord( nitfile, itemname, retrec, pos )
     
     char *nitfile;
     char *itemname;
     NitRecord *retrec;
     long int *pos;
     
{
  char          tmpitemname[ITEMNAME_SIZE];
  char          tmpalter[ITEMNAME_SIZE];
  FILE         *nitfp = ( FILE * ) NULL;
  NitRecord     nitrec;
  
  *pos = 0;
  
  if(( nitfp = OpenFile( nitfile, InfoREAD )) == ( FILE * ) NULL )
    return( INFO_FAIL );
  
  while( INFO_SUCCESS )
    {
      if( ! NitReadRecord( nitfp, &nitrec, *pos ))
        {
          CloseFile( nitfp );
          return( INFO_FAIL );
        }
      
      TerminateString( nitrec.ItemName,
                      sizeof ( nitrec.ItemName ),
                      tmpitemname );
      TerminateString( nitrec.AlternateName,
                      sizeof( nitrec.AlternateName ),
                      tmpalter );
      
      if(( stricmp( tmpitemname, itemname ) == 0 ) ||
         ( stricmp( tmpalter, itemname ) == 0 ))
        break;
      
      *pos = *pos + 1;
    }
  
  if( ! CloseFile( nitfp ))
    return( INFO_FAIL );
  
  memcpy( retrec, &nitrec, sizeof( NitRecord ));
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: NitReadRecord
  
  Purpose: This function reads an NIT record from the open file
  pointer and NIT file offset (pos).  Byte swapping, as defined by the
  macro INFO_SWAP is also preformed.  Read status is returned.  Note:
  not all record elements are byte swapped.
  
  Parameters:
  nitfp ( FILE * ) NIT file pointer
  nitrec ( DirRecord * ) NIT record
  pos ( long int ) NIT file offset
  
  Outputs:
  Function - ( short int ) Returned read status.
  
  On error, the contents of nitrec is undefined.
  
  History: 11/92  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int NitReadRecord( nitfp, nitrec, pos )
     
     FILE *nitfp;
     NitRecord *nitrec;
     long int pos;
     
{
  if( fseek( nitfp, (( long int )( sizeof( NitRecord ) * pos )), 0 ))
    return( INFO_FAIL );
  
  if( fread( nitrec, sizeof( NitRecord ), 1, nitfp ) != 1 )
    return( INFO_FAIL );
  
  INFO_BYTESWAP( &nitrec->ItemWidth, sizeof( nitrec->ItemWidth ), 1 );
  INFO_BYTESWAP( &nitrec->ItemPosition, sizeof( nitrec->ItemPosition ), 1 );
  INFO_BYTESWAP( &nitrec->OutputWidth, sizeof( nitrec->OutputWidth ), 1 );
  INFO_BYTESWAP( &nitrec->NumberDecimals, sizeof( nitrec->NumberDecimals ), 1 );
  INFO_BYTESWAP( &nitrec->ItemType, sizeof( nitrec->ItemType ), 1 );
  INFO_BYTESWAP( &nitrec->ItemNumber, sizeof( nitrec->ItemNumber ), 1 );
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: NitWriteRecord
  
  Purpose: This function writes the input NIT record to the input NIT
  file based on the record's position.  No checking of the input
  record or its file position is performed.  An error is generated if
  the NIT file cannot be opened for write, the file position cannot be
  located, an error occurs while writing to the NIT file, or a file
  close error occurs.  On success, the subject NIT file is updated
  with the input record.
  
  Parameters:
  nitfile ( char * ) INFO directory file path
  nitrec ( NitRecord * ) DR9 record
  pos ( long int ) NIT record position (record number)
  
  Outputs:
  Function - ( short int ) Returned function status
  INFO_FAIL == write error
  INFO_SUCCESS == write ok
  
  On success, the subject NIT file is updated, on error, results are
  undefined.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  11/92  Todd Stellhorn  - added byte swapping
  
  ***************************************************************************/

short int NitWriteRecord( nitfile, nitrec, pos )
     
     char *nitfile;
     NitRecord *nitrec;
     long int pos;
     
{
  FILE         *nitfp;
  
  if(( nitfp = OpenFile( nitfile, InfoWRITE )) == ( FILE * ) NULL )
    return( INFO_FAIL );
  
  if( fseek( nitfp, ( long int ) ( sizeof( NitRecord ) * pos ), 0 ))
    {
      CloseFile( nitfp );
      return( INFO_FAIL );
    }
  
  INFO_BYTESWAP( &nitrec->ItemWidth, sizeof( nitrec->ItemWidth ), 1 );
  INFO_BYTESWAP( &nitrec->ItemPosition, sizeof( nitrec->ItemPosition ), 1 );
  INFO_BYTESWAP( &nitrec->OutputWidth, sizeof( nitrec->OutputWidth ), 1 );
  INFO_BYTESWAP( &nitrec->NumberDecimals, sizeof( nitrec->NumberDecimals ), 1 );
  INFO_BYTESWAP( &nitrec->ItemType, sizeof( nitrec->ItemType ), 1 );
  INFO_BYTESWAP( &nitrec->ItemNumber, sizeof( nitrec->ItemNumber ), 1 );
  
  if(( fwrite( nitrec, ( sizeof( NitRecord )), 1, nitfp )) != 1 )
    {
      CloseFile( nitfp );
      return( INFO_FAIL );
    }
  
  return( CloseFile( nitfp ));
}
