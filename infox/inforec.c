/* @(#)inforec.c	1.1 4/13/93 */
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
  Name: inforec.c
  
  Description: Contains infolib record functions.  The following
  functions are contained in this module:
  
  InfoReadRecord
  InfoReadNextRecord
  InfoWriteRecord
  InfoWriteNextRecord
  
  History: 11/92  Todd Stellhorn  - removed functions from infofile.c
  
  10/94  Randy Deardorff - my own peculiar includes.
  
  01/95 Randy Deardorff - added stdio.h.  This is not needed unless
  you are also including rbdpaths.h.
  
  ***************************************************************************/

#include <stdio.h>
#include <rbdpaths.h>
#include INFOLIB_H
#include INFODEFS_H

/***************************************************************************
  Name: InfoReadRecord
  
  Purpose: This function reads a record from an open INFO file based
  on an InfoFile pointer and a record number.  The record is placed in
  the InfoFile's IOBuffer and the value for CurrentRecord is updated.
  An error occurs if RecordNumber is less than or equal to zero or
  greater than the current number of records.  On function error, the
  contents of the IOBuffer and the value of CurrentRecord are
  undefined.
  
  Parameters:
  ifp ( InfoFile * ) INFO file pointer
  recno ( long int ) Record number
  
  Outputs:
  Function - ( short int )  Returned read status
  INFO_FAIL == read error
  INFO_SUCCESS == read ok
  
  On success, the IOBuffer is updated with the read record and the
  value of CurrentRecord is updated -- on error, IOBuffer and
  CurrentRecord are undefined.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int InfoReadRecord( ifp, recno )
     
     InfoFile *ifp;
     long int recno;
     
{
  if( ifp == ( InfoFile * ) NULL )
    return( INFO_FAIL );
  
  if(( ifp->NumberRecords <= 0 ) || ( recno > ifp->NumberRecords ))
    return( INFO_FAIL );
  
  if( fseek( ifp->InfoFILE, ( long int )(( ifp->RecordLength ) * 
                                         ( recno - 1 )), 0 ))
    return( INFO_FAIL );
  
  if(( fread( ifp->IOBuffer, ifp->RecordLength, 1, 
             ifp->InfoFILE )) != 1 ) 
    return( INFO_FAIL );
  
  ifp->CurrentRecord = recno;
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: InfoReadNextRecord
  
  Purpose: This function reads the next record from an open INFO file.
  It is assumed that a record has already been read using
  InfoReadRecord().  The record is placed in the InfoFile's IOBuffer
  and the value for CurrentRecord is updated.  An error occurs if an
  attempt to read past the current number of records.  On function
  error, the contents of the IOBuffer and the value of CurrentRecord
  are undefined.
  
  Parameters: ifp ( InfoFile * ) INFO file pointer
  
  Outputs:
  Function - ( short int )  Returned read status
  INFO_FAIL == read error
  INFO_SUCCESS == read ok
  
  On success, the IOBuffer is updated with the read record and the
  value of CurrentRecord is updated -- on error, IOBuffer and
  CurrentRecord are undefined.
  
  History: 11/92  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int InfoReadNextRecord( ifp )
     
     InfoFile *ifp;
     
{
  return( InfoReadRecord( ifp, ( ifp->CurrentRecord + 1 )));
}


/***************************************************************************
  Name: InfoWriteRecord
  
  Purpose: This function writes a record to an open INFO file based on
  an InfoFile pointer and a record number.  The record which is
  written is contained in the InfoFile's IOBuffer.  An error occurs if
  the INFO file has been opened for read (using macro InfoREAD), or
  recno is less than or equal to zero or greater than the number of
  records + 1.  If the write operation creates a new record, the
  number of records is updated.  Write status is returned.
  
  Parameters:
  ifp ( InfoFile * ) INFO file pointer
  recno ( long int ) Record number
  
  Outputs:
  Function - ( short int )  Returned write status
  INFO_FAIL == write error
  INFO_SUCCESS == write ok
  
  On success, the IOBuffer is written to the open INFO data file and
  the number of records is updated -- on error the status of the data
  file and the number of records are undefined.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int InfoWriteRecord( ifp, recno )
     
     InfoFile *ifp;
     long int recno;
     
{
  if( ifp == ( InfoFile * ) NULL )
    return( INFO_FAIL );
  
  if( ifp->AccessFlag == InfoREAD )
    return( INFO_FAIL );
  
  if(( recno <= 0 ) || ( recno > ( ifp->NumberRecords + 1 )))
    return( INFO_FAIL );
  
  if( fseek( ifp->InfoFILE, ( long int ) (( ifp->RecordLength ) *
                                          ( recno - 1 )), 0 ))
    return( INFO_FAIL );
  
  if(( fwrite( ifp->IOBuffer, ifp->RecordLength, 1, ifp->InfoFILE )) != 1 ) 
    return( INFO_FAIL );
  
  if( recno > ifp->NumberRecords )
    ifp->NumberRecords++;
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: InfoWriteNextRecord 
  
  Purpose: This function writes a record to the next record position
  from the current record position.  The record which is written is
  contained in the InfoFile's IOBuffer.  An error occurs if the INFO
  file has been opened for read (using macro InfoREAD), or recno is
  less than or equal to zero or greater than the number of records +
  1.  If the write operation creates a new record, the number of
  records is updated.  Write status is returned.
  
  Parameters: ifp ( InfoFile * ) INFO file pointer
  
  Outputs:
  Function - ( short int )  Returned write status
  INFO_FAIL == write error
  INFO_SUCCESS == write ok
  
  On success, the IOBuffer is written to the open INFO data file and
  the number of records is updated -- on error the status of the data
  file and the number of records are undefined.
  
  History: 11/92  Todd Stellhorn  - original coding 
  
  ***************************************************************************/ 

short int InfoWriteNextRecord( ifp )
     
     InfoFile *ifp;
     
{ 
  return( InfoWriteRecord( ifp, ( ifp->CurrentRecord + 1 ))); 
}
