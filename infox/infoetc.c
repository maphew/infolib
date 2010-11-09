/* @(#)infoetc.c	1.11 2/14/94 09:29:20 */
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
  Name: infoetc.c
  
  Description: Contains infolib miscellaneous functions.  The
  following functions are contained in this module:
  
  OpenFile
  CloseFile
  CompareValues
  GetExternalPath
  IOBufferAllocate
  InfoFileFree
  
  History: 12/91  Todd Stellhorn  - original coding
  
  10/93 Russell East - mod to GetExternlPath to add support for
  external files whose external pathname contains no directory part
  
  10/94  Randy Deardorff - my own peculiar includes.
  
  01/95 Randy Deardorff - added stdio.h.  This is not needed unless
  you are also including rbdpaths.h.
  
  ***************************************************************************/

#include <stdio.h>
#include <rbdpaths.h>
#include INFOLIB_H
#include INFODEFS_H

/***************************************************************************
  Name: OpenFile
  
  Purpose: This function attempts to open a file based on the input
  file name and file access.  File access is set using the macros
  InfoREAD, InfoWRITE, InfoINIT.  If a file is opened with InfoINIT,
  the file will be truncated to zero length.  All file access types
  open files in binary mode.  On success, a file pointer is returned
  or NULL on error.
  
  Parameters:
  filename ( char * ) File name
  access ( short int ) File open access type (access type
  set using macros InfoREAD, InfoWRITE and InfoINIT) - all
  files are opened in binary mode
  
  Outputs:
  Function - ( FILE * ) Returned pointer to opened file, NULL on error
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

FILE *OpenFile( filename, access )
     
     char *filename;
     short int access;
     
{
  FILE       *fp;
  
  switch ( access )
    {
    case InfoREAD:
      
      if(( fp = fopen( filename, "rb" )) == NULL )
        return(( FILE * ) NULL );
      break;
      
    case InfoWRITE:
      
      if(( fp = fopen( filename, "r+b" )) == NULL )
        return(( FILE * ) NULL );
      break;
      
    case InfoINIT:
      
      if(( fp = fopen( filename, "w+b" )) == NULL )
        return(( FILE * ) NULL );
      break;
      
    default:
      return(( FILE * ) NULL );
    }
  
  return( fp );
}


/***************************************************************************
  Name: CloseFile
  
  Purpose: This function closes an open file pointed to by the input
  file pointer and returns the file close status.
  
  Parameters:
  fp ( FILE * ) File pointer
  
  Outputs:
  Function - ( short int ) Returned close status
  INFO_FAIL == file close error
  INFO_SUCCESS == file close ok
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int CloseFile( fp )
     
     FILE *fp;
     
{
  if( fclose( fp ) != 0 )
    return( INFO_FAIL );
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: CompareValues
  
  Purpose: This function compares two values and returns a comparison
  status.  The first value is contained in either str or val and the
  second value is contained in the ifp's IOBuffer.  The value from the
  IOBuffer is decoded based on the input item definition and used for
  the comparison.  This function can only be used with C, I, or B INFO
  data types (macros INFO_CHARACTER_TYPE, INFO_INTEGER_TYPE, or
  INFO_BINARY_TYPE).  An error will be generated if any other data
  type is used.  The comparison status value is returned via cmp.
  Function status is also returned.
  
  Parameters:
  ifp ( InfoFile * ) INFO file pointer
  idp ( InfoItemDef * ) INFO item definition
  str ( char * ) Search string
  ival ( long int ) Search integer value
  cmp ( short int * ) Compare status
  
  Outputs:
  Function - ( short int )  Returned function status
  INFO_FAIL == compare error
  INFO_SUCCESS == compare ok
  
  Returned comparison status (cmp):
  < 0 == str or ival is less than IOBuffer value
  == 0 == str or ival is equal to IOBuffer value
  > 0 == str or ival is greater than IOBuffer value
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int CompareValues( ifp, idp, str, ival, cmp )
     
     InfoFile *ifp;
     InfoItemDef *idp;
     char *str;
     long int ival;
     short int *cmp;
     
{
  char               retstr[TMPRECORD_SIZE];
  double             dval;
  long int           cmpval;
  
  if( ! InfoDecode( ifp, idp, retstr, &dval ))
    return( INFO_FAIL );
  
  switch( idp->ItemType )
    {
    case INFO_CHARACTER_TYPE :
      
      CompressString( retstr, idp->ItemWidth );
      *cmp = strcmp( str, retstr );
      break;
      
    case INFO_INTEGER_TYPE   :
    case INFO_BINARY_TYPE    :
      
      cmpval = ival - ( long int ) dval;
      
      if( ! cmpval )
        *cmp = 0;
      else
        if( cmpval < 0 )
          *cmp = -1;
        else
          *cmp = 1;
      break;
      
      default :
        
        return( INFO_FAIL );
    }
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: GetExternalPath
  
  Purpose: This function returns the pathname string from an INFO
  externalled data file.  Function assumes a full pathname to an INFO
  data file (xxx###dat file) and the full pathname to the INFO
  directory.  The function reads the pathname string from the data
  file and tests if the path name is a relative path (such as
  generated with ARC/INFO Rev6.0).  If a relative path is found, an
  attempt is made to generate a full pathname using the pathname to
  the INFO directory.  This function does not expand relative paths
  which do not start with "..".  Expanded file pathnames are not
  guaranteed to open the externalled data file. Function status and a
  fully expanded data file path are returned.
  
  Parameters:
  datafilepath ( char * ) External data file pathname
  infofilepath ( char * ) INFO directory pathname
  extpath ( char * ) Expanded external data file
  pathname
  
  Outputs:
  Function - ( short int ) Returned function status
  INFO_FAIL == status error
  INFO_SUCCESS == status ok
  
  Also returned is expanded external data file pathname (extpath).
  
  History: 12/91  Todd Stellhorn  - original coding
  
  10/93 Russell East - added support for external files that exist
  within the info directory, and whose external pathname does not
  contain a directory part
  
  ***************************************************************************/

short int GetExternalPath( datafilepath, infofilepath, extpath )
     
     char *datafilepath;
     char *infofilepath;
     char *extpath;
     
{
  char        tmpstr[PATH_SIZE];
  char        tmppath[PATH_SIZE];
  char       *strptr;
  FILE       *fp = ( FILE * ) NULL;
  
  if(( fp = fopen( datafilepath, "r" )) == NULL )
    return( INFO_FAIL );
  
  if(( fgets( tmpstr, sizeof( tmpstr ), fp )) == NULL )
    {
      fclose( fp );
      return( INFO_FAIL );
    }
  
  if( fclose( fp ) != 0 )
    return( INFO_FAIL );
  
  /* terminate and remove trailing blanks from string and convert 
     string to lowercase 
     */
  
  TerminateString( tmpstr, sizeof( tmpstr ), tmppath );
  StringtoLower( tmppath );
  
  /* expand relative path to fullpath */
  
  if(( tmppath[0] == '.' ) && ( tmppath[1] == '.' ))
    {
      /* extract directory path from info file pathname, first
         copy info file pathname into temp string buffer */
      strcpy( tmpstr, infofilepath );
      
      /* point to end of string buffer */
      strptr = &tmpstr[ strlen( tmpstr ) - 1 ];
      
      /* search backwards until operating system separator found */
      while( *strptr != OPSYS_SEP[0] )
        strptr--;
      
      /* terminate string and copy result */
      *strptr = '\0';
      
      sprintf( extpath, "%s%s", tmpstr, &tmppath[2] );
    }
  else {
    
    /* check for the stored name is just a simple file name,
       ie. the case where the external file resides within the INFO directory 
       */
    for( strptr = &tmppath[ strlen( tmppath ) ]; strptr != tmppath; ) {
      strptr--;
      if( *strptr == OPSYS_SEP[0] ) break;
    }
    
    if( *strptr == OPSYS_SEP[0] )
      
      /* assume the stored name *is* a full pathname */
      strcpy( extpath, tmppath );
    
    else {
      /* need to prepend the info directory name for the external pathname 
       */
      sprintf( extpath, "%s%s%s", infofilepath, OPSYS_SEP, tmppath );
      
    }
  }
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: IOBufferAllocate
  
  Purpose: This function allocates a character buffer based on an
  input buffer size.  Allocated buffer is initialized with blanks ('
  ').  Pointer to buffer is returned or NULL on error.
  
  Parameters:
  size ( short int ) Character buffer size
  
  Outputs:
  Function - ( char * ) Character pointer to allocated
  buffer or NULL on error.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

char *IOBufferAllocate( size )
     
     short int size;
     
{
  char          *buf;
  
  if(( buf = ( char * ) malloc( size )) == NULL )
    return(( char * ) NULL );
  
  memset( buf, ' ', size );
  
  return( buf );
}


/***************************************************************************
  Name: InfoFileFree
  
  Purpose: This function returns INFO file pointer memory to free
  memory.
  
  Parameters: ifp ( InfoFile * ) InfoFile pointer
  
  Outputs: Function - ( void )
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

void InfoFileFree( ifp )
     
     InfoFile *ifp;
     
{
  if( ifp == ( InfoFile * ) NULL )
    return;
  
  if( ifp->FileName != ( char * ) NULL )
    free( ifp->FileName );
  
  if( ifp->DirFilePath != ( char * ) NULL )
    free( ifp->DirFilePath );
  
  if( ifp->NitFilePath != ( char * ) NULL )
    free( ifp->NitFilePath );
  
  if( ifp->IOBuffer != ( char * ) NULL )
    free( ifp->IOBuffer );
  
  free( ifp );
}
