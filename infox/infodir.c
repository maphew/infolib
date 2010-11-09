/* @(#)infodir.c	1.8 4/13/93 */
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
 *                            Todd Stellhorn
 *                            ESRI
 */

/***************************************************************************
  
  Name: infodir.c

  Description: Contains infolib directory file functions.  The following
  functions are contained in this module:
  
  DirGetRecord
  DirReadRecord
  DirWriteRecord
  
  History: 12/91  Todd Stellhorn  - original coding
  
  10/94  Randy Deardorff - my own peculiar includes.
  
  01/95  Randy Deardorff - added stdio.h.  This is not needed
  unless you are also including rbdpaths.h.
  
 ***************************************************************************/

#include <stdio.h>
#include <rbdpaths.h>
#include INFOLIB_H
#include INFODEFS_H
#include INFOSWAP_H
#undef   DEBUGGING

/***************************************************************************
  Name: DirGetRecord
  
  Purpose: This function searches the input DR9 file for a record
  entry of type recstat (set using macros DirEMPTY or DirNotEMPTY) and
  matching the input INFO external file name.  The matched record and
  its file position are returned.  If recstat is DirEMPTY, the first
  empty record is returned.  If no empty records are found, an error
  is generated and pos is set to a negative number which equals the
  size of the DR9 file.  This allows for appending of new records onto
  the end of the DR9 file.
  
  Parameters:
  dirfile ( char * ) INFO DR9 file path
  recstat ( short int ) Directory record type (DirEMPTY or DirNotEMPTY)
  filename ( char * ) INFO external file name
  retrec ( DirRecord * ) Directory record buffer
  pos ( long int * ) Directory record file position (record number)
  
  Outputs:
  Function - ( short int ) Returned function status
  INFO_FAIL == error returning record
  INFO_SUCCESS == matching record found
  
  On error, recstat and pos are undefined unless pos is less than 0,
  then ( pos * -1 ) indicates the last directory entry position.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  01/95 Randy Deardorff - Added reference to MacroPROMOTE to determine
  whether to promote recstat to long.
  
  ***************************************************************************/

short int DirGetRecord( dirfile, recstat, filename, retrec, pos )
     
     char *dirfile;
#if MacroPROMOTE
     long int recstat; /* recstat set to long int due to macro promotion */
#else
     int recstat;
#endif
     char *filename;
     DirRecord *retrec;
     long int *pos;
     
{
  char          internalname[INTERNALNAME_SIZE];
  FILE         *dirfp = ( FILE * ) NULL;
  DirRecord     dirrec;
  
  *pos = 0;
  
  if(( dirfp = OpenFile( dirfile, InfoREAD )) == ( FILE * ) NULL )
    return( INFO_FAIL );
  
  while( INFO_SUCCESS ) /* loop until end of file or matching record found 
                         */
    {
      if( ! DirReadRecord( dirfp, &dirrec, *pos ))
        {
          /* test for end of file on read error */
          if( feof( dirfp ))
            {
              /* if at end of file, flag file position but still
                 return error */
              if( ! CloseFile( dirfp ))
                return( INFO_FAIL );
              
              *pos = *pos * -1;
              return( INFO_FAIL );
            }
          else
            {
              CloseFile( dirfp );
              return( INFO_FAIL );
            }
        }
      
      if(( dirrec.EmptyFlag == recstat ) && 
         ( dirrec.FileType == DirDataFileTYPE ))
        {
          if( recstat == DirNotEMPTY )
            {
              TerminateString( dirrec.InternalName,
                              sizeof( dirrec.InternalName ), internalname );
              
              if( ! stricmp( internalname, filename ))
                break;
            }
          else
            break;
        }
      *pos = *pos + 1;
    }
  
  if( ! CloseFile( dirfp ))
    return( INFO_FAIL );
  
  memcpy( retrec, &dirrec, sizeof( DirRecord ));
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: DirReadRecord
  
  Purpose: This function reads a DR9 record from the open file pointer
  and DR9 file offset (pos).  Byte swapping, as defined by the macro
  INFO_SWAP is also preformed.  Read status is returned.  Note: not
  all record elements are byte swapped.
  
  Parameters:
  dirfp ( FILE * ) DR9 file pointer
  dirrec ( DirRecord * ) DR9 record
  pos ( long int ) DR9 file offset
  
  Outputs:
  Function - ( short int ) Returned read status.
  
  On error, the contents of dirrec is undefined.
  
  History: 11/92  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int DirReadRecord( dirfp, dirrec, pos )
     
     FILE *dirfp;
     DirRecord *dirrec;
     long int pos;
     
{
  if( fseek( dirfp, (( long int )( sizeof( DirRecord ) * pos )), 0 ))
    return( INFO_FAIL );
  
  if( fread( dirrec, sizeof( DirRecord ), 1, dirfp ) != 1 )
    return( INFO_FAIL );
  
  INFO_BYTESWAP( &dirrec->NumberItems, sizeof( dirrec->NumberItems ), 1 );
  INFO_BYTESWAP( &dirrec->RecordLength, sizeof( dirrec->RecordLength ), 1 );
  INFO_BYTESWAP( &dirrec->EmptyFlag, sizeof( dirrec->EmptyFlag ), 1 );
  INFO_BYTESWAP( &dirrec->NumberRecords, sizeof( dirrec->NumberRecords ), 1);
  INFO_BYTESWAP( &dirrec->FileType, sizeof( dirrec->FileType ), 1 );
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: DirWriteRecord
  
  Purpose: This function writes the input directory record to the
  input DR9 file based on the record's position.  No checking of the
  input record or its file position is performed.  An error is
  generated if the directory file cannot be opened for write, the file
  position cannot be located, an error occurs while writing to the DR9
  file, or a file close error occurs.  On success, the subject DR9
  file is updated with the input record.
  
  Parameters:
  dirfile ( char * ) INFO directory file path
  dirrec ( DirRecord * ) DR9 record
  pos ( long int ) DR9 record position (record number)
  
  Outputs:
  Function - ( short int ) Returned function status
  INFO_FAIL == write error
  INFO_SUCCESS == write ok
  
  On success, the subject DR9 file is updated, on error, results are
  undefined.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  11/92  Todd Stellhorn  - added byte swapping
  
  ***************************************************************************/

short int DirWriteRecord( dirfile, dirrec, pos )
     
     char *dirfile;
     DirRecord *dirrec;
     long int pos;
     
{
  FILE         *dirfp;
  
  if(( dirfp = OpenFile( dirfile, InfoWRITE )) == ( FILE * ) NULL )
    return( INFO_FAIL );
  
  if( fseek( dirfp, ( long int ) ( sizeof( DirRecord ) * pos ), 0 ))
    {
      CloseFile( dirfp );
      return( INFO_FAIL );
    }
  
  INFO_BYTESWAP( &dirrec->NumberItems, sizeof( dirrec->NumberItems ), 1 );
  INFO_BYTESWAP( &dirrec->RecordLength, sizeof( dirrec->RecordLength ), 1 );
  INFO_BYTESWAP( &dirrec->EmptyFlag, sizeof( dirrec->EmptyFlag ), 1 );
  INFO_BYTESWAP( &dirrec->NumberRecords, sizeof( dirrec->NumberRecords ), 1 );
  INFO_BYTESWAP( &dirrec->FileType, sizeof( dirrec->FileType ), 1 );
  
  if(( fwrite( dirrec, sizeof( DirRecord ), 1, dirfp )) != 1 )
    {
      CloseFile( dirfp );
      return( INFO_FAIL );
    }
  
  return( CloseFile( dirfp ));
}
