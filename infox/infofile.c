/* @(#)infofile.c	1.14 2/14/94 09:30:17 */
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
  Name: infofile.c
  
  Description: Contains infolib file functions.  The following
  functions are contained in this module:
  
  InfoOpenFile
  InfoCloseFile
  InfoCreateFile
  InfoDeleteFile
  InfoFileExists
  InfoFileFlush
  InfoNumberRecords
  InfoNumberItems
  InfoRecordLength
  InfoOpenForWrite
  InfoCurrentRecord
  
  History: 12/91  Todd Stellhorn  - original coding
  
  12/94 Randy Deardorff -  My own peculiar includes.
  
  12/94 Randy Deardorff - Changed the way the functions figure out
  whether they are looking at an 8.3 or pre-8.3 workspace.  Added
  file.h and calls to access system function.  These changes apply to
  InfoCreateFile, InfoOpenFile, InfoDeleteFile, and InfoFileExists.
  Renamed variable "access" to "accessmode".  Changed InfoCreateFile
  so that tmpext gets correctly set to "arcNNNN" rather than "arcNNN"
  if dir type is 8.3.
  
  01/95 Randy Deardorff - added stdio.h.  This is not needed unless
  you are also including rbdpaths.h.  Include io.h instead of file.h
  on BORLAND_C.

  ***************************************************************************/

#include <stdio.h>
#include <rbdpaths.h>

#if BORLAND_C
#include <io.h>
#else
#include <sys/file.h>
#endif

#ifndef F_OK
#define F_OK 0
#define X_OK 1
#define W_OK 2
#define R_OK 4
#endif

#include INFOLIB_H
#include INFODEFS_H

dirtype_t dirtype;

/***************************************************************************
  Name: InfoOpenFile
  
  Purpose: This function opens an INFO file based on an input INFO
  file pathname and access type.  Access types are set using the
  macros InfoREAD, InfoWRITE and InfoINIT (InfoINIT will truncate the
  data file to zero length on open).  ARC/INFO style INFO file
  pathnames are supported, such as:
  
  /sleet1/todd/infolib/info:arc:test
  where:
  /sleet1/todd/infolib/info - full pathname to INFO
  directory
  : or ! - INFO path separator
  test - external name of the INFO file
  
  InfoOpenFile returns an InfoFile pointer or NULL on error.
  
  Parameters:
  filename ( char * ) INFO file name
  accessmode ( short int ) File access type (access types
  are defined by macros InfoREAD, InfoWRITE and InfoINIT)
  
  Outputs:
  Function - ( InfoFile * ) Returned pointer to open INFO file
  on success or NULL on error.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  12/94 Randy Deardorff - Fixed 8.3 v pre-8.3 logic
  
  ***************************************************************************/

InfoFile *InfoOpenFile( name, accessmode )
     
     char *name;
     short int accessmode;
     
{
  char          infodir[PATH_SIZE];
  char          username[USERNAME_SIZE];
  char          filename[INTERNALNAME_SIZE];
  char          dirpath[PATH_SIZE];
  char          extname[EXTNAME_SIZE];
  char          extpath[PATH_SIZE];
  char          nitpath[PATH_SIZE];
  char          tmppath[PATH_SIZE];
  DirRecord     dirrec;
  long int      dirpos = 0;
  InfoFile     *ifp = ( InfoFile * ) NULL;
  
  if( ! InfoParsePath( name, infodir, username, filename ))
    return(( InfoFile * ) NULL );
  
  MAKEDIROLD (dirpath, infodir, username);
  if (access(dirpath,F_OK)==0)
    {
      if (!DirGetRecord (dirpath,DirNotEMPTY,filename,&dirrec,&dirpos))
        return(( InfoFile * ) NULL );
    }
  else
    {
      MAKEDIR83 (dirpath, infodir, username);
      if (access(dirpath,F_OK)==0)
        {
          if (!DirGetRecord (dirpath,DirNotEMPTY,filename,&dirrec,&dirpos))
            return(( InfoFile * ) NULL );
        }
    }
  
  if( dirrec.RecordLength == 0 )  /* zero record length, return error */
    return(( InfoFile * ) NULL );
  
  TerminateString( dirrec.ExternalName, sizeof( dirrec.ExternalName ),
                  extname );
  
  MAKEDAT (extpath, infodir, extname);
  MAKENIT (nitpath, infodir, extname);
  
  /* test for external data file */
  if( dirrec.ExternalFlag[0] == 'X' )
    {
      strcpy( tmppath, extpath );
      if( ! GetExternalPath( tmppath, infodir, extpath ))
        return(( InfoFile * ) NULL );
    }
  
  if(( ifp = ( InfoFile * )malloc( sizeof ( InfoFile ))) == NULL )
    return(( InfoFile * ) NULL );
  
  ifp->FileName = ( char * ) NULL;
  ifp->DirFilePath = ( char * ) NULL;
  ifp->NitFilePath = ( char * ) NULL;
  ifp->InfoFILE = ( FILE * ) NULL;
  ifp->NumberItems = dirrec.NumberItems;
  ifp->RecordLength = dirrec.RecordLength;
  ifp->NumberRecords = dirrec.NumberRecords;
  ifp->AccessFlag = accessmode;
  ifp->CurrentRecord = 0;
  ifp->IOBuffer = ( char * ) NULL;
  
  if(( ifp->FileName = ( char * )malloc( strlen( filename ) + 1 )) == NULL )
    {
      InfoFileFree( ifp );
      return(( InfoFile * ) NULL );
    }
  strcpy( ifp->FileName, filename );
  
  if(( ifp->DirFilePath = ( char * )malloc( strlen( dirpath ) + 1 )) == NULL )
    {
      InfoFileFree( ifp );
      return(( InfoFile * ) NULL );
    }
  strcpy( ifp->DirFilePath, dirpath );
  
  if(( ifp->NitFilePath = ( char * )malloc( strlen( nitpath ) + 1 )) == NULL )
    {
      InfoFileFree( ifp );
      return(( InfoFile * ) NULL );
    }
  strcpy( ifp->NitFilePath, nitpath );
  
  if(( ifp->InfoFILE = OpenFile( extpath, accessmode )) == ( FILE * ) NULL )
    {
      InfoFileFree( ifp );
      return(( InfoFile * ) NULL );
    }
  
  if( accessmode == InfoINIT )    /* if init, update number of records */
    ifp->NumberRecords = 0;
  
  if(( ifp->IOBuffer = IOBufferAllocate( ifp->RecordLength )) == NULL )
    {
      InfoFileFree( ifp );
      return(( InfoFile * ) NULL );
    }
  
  return( ifp );
}


/***************************************************************************
  Name: InfoCloseFile
  
  Purpose: This function closes an open INFO file and returns the
  InfoFile memory to free memory.  If the file was opened using macros
  InfoWRITE or InfoINIT, DR9 file is updated with the current number
  of records.  INFO file close status is returned.
  
  Parameters:
  ifp ( InfoFile * ) INFO file pointer
  
  Outputs:
  Function - ( short int )  INFO file close status
  INFO_FAIL == file close error
  INFO_SUCCESS == file close ok
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int InfoCloseFile( ifp )
     
     InfoFile *ifp;
     
{
  if( ! CloseFile( ifp->InfoFILE ))
    return( INFO_FAIL );
  
  if( ifp->AccessFlag != InfoREAD )
    {
      long int         dirpos = 0;
      DirRecord        dirrec;
      
      if( ! DirGetRecord( ifp->DirFilePath, DirNotEMPTY,
                         ifp->FileName, &dirrec, &dirpos ))
        return( INFO_FAIL );
      
      dirrec.NumberRecords = ifp->NumberRecords;
      
      if( ! DirWriteRecord( ifp->DirFilePath, &dirrec, dirpos ))
        return( INFO_FAIL );
    }
  
  InfoFileFree( ifp );
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: InfoCreateFile
  
  Purpose: Creates a new empty INFO file with either an empty item
  template or an item template copied from an input open INFO file.
  The item file is empty if a NULL INFO file pointer is passed as the
  second argument.  Empty INFO files may have items added using
  InfoDefineItem().  File creation status is returned.
  
  Parameters:
  name ( char * ) Input INFO file pathname
  template ( InfoFile * ) INFO file pointer or NULL
  
  Outputs: Function - ( short int ) Returned file creatation status
  
  History: 11/92  Todd Stellhorn  - original coding
  
  12/94 Randy Deardorff - Fixed 8.3 v pre-8.3 logic
  
  ***************************************************************************/

short int InfoCreateFile( name, template )
     
     char *name;
     InfoFile *template;
     
{
  char          infodir[PATH_SIZE];
  char          username[USERNAME_SIZE];
  char          filename[INTERNALNAME_SIZE];
  char          dirpath[PATH_SIZE];
  char          extpath[PATH_SIZE];
  char          nitpath[PATH_SIZE];
  char          tmppath[PATH_SIZE];
  char          tmpext[TMPSTRING_SIZE];
  DirRecord     dirrec;
  long int      dirpos = 0;
  short int     dirstat = 0;
  FILE         *tmpfp = ( FILE * ) NULL;
  FILE         *tmpfp2 = ( FILE * ) NULL;
  NitRecord     nitrec;
  
  if( InfoFileExists( name ))
    return( INFO_FAIL );
  
  if( ! InfoParsePath( name, infodir, username, filename ))
    return( INFO_FAIL );
  
  /* the following conditions can be returned from DirGetRecord:
     - one or more records in DR9, empty record found
     dirstat == INFO_SUCCESS
     pos     == DR9 record position
     (use returned record)
     - single record in DR9, but record is empty 
     dirstat == INFO_SUCCESS
     pos     == 0
     (use returned record)
     - empty DR9 (newly created)
     dirstat == INFO_FAIL
     pos     == 0
     (generate new record)
     - one or more non-empty records
     dirstat == INFO_FAIL
     pos     == ( DR9 record position * -1 )
     (generate new record)
     - out right error
     dirstat == INFO_FAIL
     pos     >= 0
     (return error)
     
     given the above, the only way to tell the difference between 
     cases 3 and 5 will be when we attempt to write the new record.
     hopfully we can catch the error there. 
     */
  
  MAKEDIROLD (dirpath, infodir, username);
  if (access(dirpath,F_OK)==0)
    dirstat = DirGetRecord (dirpath,DirEMPTY,filename,&dirrec,&dirpos);
  else
    {
      MAKEDIR83 (dirpath, infodir, username);
      if (access(dirpath,F_OK)==0)
        dirstat = DirGetRecord (dirpath,DirEMPTY,filename,&dirrec,&dirpos);
      else
        return(INFO_FAIL);
    }
  
  if(( ! dirstat ) && ( dirpos <= 0 ))	  
    {
      /* convert DR9 position */
      dirpos = dirpos * -1;
      
      memset( &dirrec, ' ', sizeof( dirrec ));
      
      StringtoUpper( username );
      
      if (dirtype==Old)
        sprintf( tmpext, "%s%03ld  ", username, dirpos );
      else
        sprintf( tmpext, "%s%04ld ", username, dirpos );
      
      strncpy( dirrec.ExternalName, tmpext, 8 );
      
      /* Level0Password init above */
      dirrec.PrintWidth = 132;
      dirrec.Filler1 = 0;
      dirrec.FileType = 0;
      dirrec.Filler2 = 0;
      /* Filler3 init above */
    }
  
  StringtoUpper( filename );
  ExpandString( filename, 32 );
  strncpy( dirrec.InternalName, filename, 32 );
  
  dirrec.NumberItems = 0;
  dirrec.RecordLength = 0;
  dirrec.EmptyFlag = 0;
  dirrec.NumberRecords = 0;
  
  dirrec.ExternalFlag[0] = ' ';
  dirrec.ExternalFlag[1] = ' ';
  
  /* create empty data and NIT files */
  TerminateString( dirrec.ExternalName, sizeof( dirrec.ExternalName ),
                  tmpext );
  
  MAKEDAT (extpath, infodir, tmpext);
  MAKENIT (nitpath, infodir, tmpext);
  
  /* create empty data file */
  if(( tmpfp = OpenFile( extpath, InfoINIT )) == ( FILE * ) NULL )
    return( INFO_FAIL );
  
  if( ! CloseFile( tmpfp ))
    return( INFO_FAIL );
  
  /* create nit file */
  if(( tmpfp = OpenFile( nitpath, InfoINIT )) == ( FILE * ) NULL )
    return( INFO_FAIL );
  
  /* if template file specified - copy template */
  if( template )
    {
      /* copy existing nit to new file */ 
      if(( tmpfp2 = OpenFile( template->NitFilePath, InfoREAD )) == ( FILE 
                                                                     * ) NULL )
        return( INFO_FAIL );
      
      while(( fread( &nitrec, sizeof( NitRecord ), 1, tmpfp2 )) == 1 )
        {
          if(( fwrite( &nitrec, ( sizeof( NitRecord )), 1, tmpfp )) != 1 )
            {
              CloseFile( tmpfp );
              CloseFile( tmpfp2 );
              return( INFO_FAIL );
            }
        }
      
      if( ! CloseFile( tmpfp2 ))
        return( INFO_FAIL );
      
      /* update dr9 record */
      dirrec.NumberItems = template->NumberItems;
      dirrec.RecordLength = template->RecordLength;
    }  
  
  if( ! CloseFile( tmpfp ))
    return( INFO_FAIL );
  
  return( DirWriteRecord( dirpath, &dirrec, dirpos ));
}


/***************************************************************************
  Name: InfoDeleteFile
  
  Purpose: This function deletes a closed INFO file based on the
  file's external name.  If the INFO file is an external file, only
  the internal NIT and DAT files are deleted -- the external file is
  not removed.  File deletion status is returned.
  
  Parameters:
  name ( char * ) INFO file name
  
  Outputs:
  Function - ( short int )  INFO file delete status
  INFO_FAIL == file delete error
  INFO_SUCCESS == file delete ok
  
  History: 11/92  Todd Stellhorn  - original coding
  
  12/94 Randy Deardorff - Fixed 8.3 v pre-8.3 logic
  
  ***************************************************************************/

short int InfoDeleteFile( name )
     
     char *name;
     
{
  char          infodir[PATH_SIZE];
  char          username[USERNAME_SIZE];
  char          filename[INTERNALNAME_SIZE];
  char          dirpath[PATH_SIZE];
  char          extname[EXTNAME_SIZE];
  char          extpath[PATH_SIZE];
  char          nitpath[PATH_SIZE];
  DirRecord     dirrec;
  long int      dirpos = 0;
  
  if( ! InfoParsePath( name, infodir, username, filename ))
    return( INFO_FAIL );
  
  MAKEDIROLD (dirpath, infodir, username);
  if (access(dirpath,F_OK)==0)
    {
      if (!DirGetRecord (dirpath,DirNotEMPTY,filename,&dirrec,&dirpos))
        return( INFO_FAIL );
    }
  else
    {
      MAKEDIR83 (dirpath, infodir, username);
      if (access(dirpath,F_OK)==0)
        {
          if (!DirGetRecord (dirpath,DirNotEMPTY,filename,&dirrec,&dirpos))
            return( INFO_FAIL );
        }
    }
  
  TerminateString( dirrec.ExternalName, sizeof( dirrec.ExternalName ),
                  extname );
  
  MAKEDAT (extpath, infodir, extname);
  MAKENIT (nitpath, infodir, extname);
  
  /* remove DAT and NIT files */
  unlink( extpath );
  unlink( nitpath );
  
  /* set the DR9 record's empty flag and write record */
  dirrec.EmptyFlag = DirEMPTY;
  
  return( DirWriteRecord( dirpath, &dirrec, dirpos ));
}


/***************************************************************************
  Name: InfoFileExists
  
  Purpose: This function tests for the existence of the input INFO.
  The test is performed by searching the DR9 file for a non-empty
  record with a matching file name.  A valid return does not guarantee
  that the INFO file can successfully be opened, only that the file
  name exists in a non-empty record in the DR9 file.  Additionally, an
  invalid return indicates either that the file does not exist or that
  an error occurred while searching the DR9 file.  In any event,
  something is wrong with the input InfoFilePath or the DR9 file has
  been corrupted so we return an error.
  
  Parameters: name ( char * ) INFO file pathname
  
  Outputs:
  Function - ( short int ) Returned file existence status or error
  INFO_FAIL == file does not exist or error
  INFO_SUCCESS == file exists (see discussion above)
  
  History: 12/91  Todd Stellhorn  - original coding
  
  12/94 Randy Deardorff - Fixed 8.3 v pre-8.3 logic
  
  ***************************************************************************/

short int InfoFileExists( name )
     
     char *name;
     
{
  char          infodir[PATH_SIZE];
  char          username[USERNAME_SIZE];
  char          filename[INTERNALNAME_SIZE];
  char          dirpath[PATH_SIZE];
  DirRecord     dirrec;
  long int      dirpos = 0;
  short         stat;
  
  if( ! InfoParsePath( name, infodir, username, filename ))
    return( INFO_FAIL );
  
  MAKEDIROLD (dirpath, infodir, username);
  if (access(dirpath,F_OK)==0)
    stat = DirGetRecord (dirpath,DirNotEMPTY,filename,&dirrec,&dirpos);
  else
    {
      MAKEDIR83 (dirpath, infodir, username);
      if (access(dirpath,F_OK)==0)
        stat = DirGetRecord (dirpath,DirNotEMPTY,filename,&dirrec,&dirpos);
      else
        stat=INFO_FAIL;
    }
  
  return (stat);
}


/***************************************************************************
  Name: InfoFileFlush
  
  Purpose: This function flushes an open INFO file pointed to by ifp.
  An error is generated if the INFO file is not open for write.
  
  Parameters:
  ifp ( InfoFile * ) INFO file pointer
  
  Outputs:
  Function - ( short int )  Returned flush
  INFO_FAIL == flush error
  INFO_SUCCESS == flush ok
  
  On success, the stream assoicated with ifp is flushed.
  
  History: 11/92  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int InfoFileFlush( ifp )
     
     InfoFile *ifp;
     
{
  if( ifp->AccessFlag == InfoREAD )
    return( INFO_FAIL );
  
  fflush( ifp->InfoFILE );
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: InfoNumberRecords
  
  Purpose: This function returns the number of records in the open INFO file.
  
  Parameters: ifp ( InfoFile * ) INFO file pointer
  
  Outputs: Function - ( long int )  Returned number of records
  
  History: 11/92  Todd Stellhorn  - original coding
  
  ***************************************************************************/

long int InfoNumberRecords( ifp )
     
     InfoFile *ifp;
     
{
  return( ifp->NumberRecords );
}


/***************************************************************************
  Name: InfoNumberItems
  
  Purpose: This function returns the number of items in the open INFO file.
  
  Parameters: ifp ( InfoFile * ) INFO file pointer 
  
  Outputs: Function - ( short int )  Returned number of items
  
  History: 11/92  Todd Stellhorn  - original coding 
  
  ***************************************************************************/ 

short int InfoNumberItems( ifp )
     
     InfoFile *ifp;   
     
{ 
  return( ifp->NumberItems ); 
}


/***************************************************************************
  Name: InfoRecordLength
  
  Purpose: This function returns the INFO file record length.
  
  Parameters: ifp ( InfoFile * ) INFO file pointer
  
  Outputs: Function - ( long int )  Returned record length
  
  History: 11/92  Todd Stellhorn  - original coding 
  
  ***************************************************************************/ 

long int InfoRecordLength( ifp ) 
     
     InfoFile *ifp;   
     
{ 
  return( ifp->RecordLength ); 
}


/***************************************************************************
  Name: InfoNumberRecords
  
  Purpose: This function returns whether the input file is open for write.
  
  Parameters: ifp ( InfoFile * ) INFO file pointer 
  
  Outputs: Function - ( short int )  Returned open for write status
  
  History: 11/92  Todd Stellhorn  - original coding 
  
  ***************************************************************************/ 

short int InfoIsOpenForWrite( ifp ) 
     
     InfoFile *ifp;   
     
{ 
  if( ifp->AccessFlag == InfoREAD )
    return( INFO_FAIL );
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: InfoCurrentRecord
  
  Purpose: This function returns the current record number.
  
  Parameters: ifp ( InfoFile * ) INFO file pointer 
  
  Outputs: Function - ( long int )  Returned current record number
  
  History: 11/92  Todd Stellhorn  - original coding 
  
  ***************************************************************************/ 

long int InfoCurrentRecord( ifp ) 
     
     InfoFile *ifp;   
     
{ 
  return( ifp->CurrentRecord ); 
}
