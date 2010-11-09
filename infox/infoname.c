/* @(#)infoname.c	1.3 2/14/94 09:34:11 */
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
  Name: infoname.c
  
 Description: Contains infolib name generation and minulation
 functions.  The following functions are contained in this module:
 
 InfoParsePath
 InfoCoverFileName
 InfoTempName
 
 History: 11/92  Todd Stellhorn  - original coding
 
 10/93 Russell East - fix InfoParsePath() to properly handle INFO path
 names; the resulting path was incorrect.
 
 10/94  Randy Deardorff - my own peculiar includes.
 
 01/95 Randy Deardorff - added stdio.h.  This is not needed unless you
 are also including rbdpaths.h.

 07/13/98 Randy Deardorff - Fixed InfoParsePath() to correctly handle PC
 style pathnames with drive letter followed by a colon.

 ***************************************************************************/

#include <stdio.h>
#include <rbdpaths.h>
#include INFOLIB_H
#include INFODEFS_H

/***************************************************************************
  Name: InfoParsePath
  
  Purpose: This function parses an the input pathname into its
  pathname components.  ARC/INFO style INFO file pathnames are
  supported, such as:
  
  /sleet1/todd/infolib/info:arc:test
  where:
  /sleet1/todd/infolib/info - full pathname to INFO
  directory
  : or ! - INFO path separator 
  test - external name of the INFO file
  
  This function returns the full pathname of the INFO directory, the
  user name (default value is set using DEFAULT_USER), and the
  external INFO file name.  If pathname and user name are not
  contained in the input string (delimited by ":" or "!") default
  values are added.
  
  Parameters:
  str ( char * ) Pointer to input string
  path ( char * ) INFO directory pathname
  user ( char * ) INFO user name
  filename ( char * ) INFO external filename
  
  Outputs:
  Function - ( short int ) Returned function status
  INFO_FAIL == InfoParsePath error
  INFO_SUCCESS == InfoParsePath ok
  
  Function also returns the full INFO directory pathname, the INFO
  user name, and the INFO external filename.
  
  History: 11/92 Todd Stellhorn - original coding based on
  ParsePathName
  
  10/93 Russell East - mod to correct problems with pathnames
  
  07/13/98 Randy Deardorff - Fixed for PC style pathnames that can contain
  drive letter followed by a colon.  Was incorrectly interpreting the colon
  as an info path separator.

  ***************************************************************************/

short int InfoParsePath( str, path, user, filename )
     
     char *str;
     char *path;
     char *user;
     char *filename;
     
{
  short int    cnt = 0;
  char        *sp, *spx;
  char         buf[PATH_SIZE];
  char         delimiter;
  
  spx = str;
  
  /* move pointer past drive leter colon backslash so as not to confuse
     the delimiter counting code below. */
  
#ifdef DOS
  if (strlen(str) > 1)
    if (str[1]==':')
      {
        spx++;
        spx++;
        if (strlen(str) > 2)
          if (str[2]=='\\')
            spx++;
      }
#endif
  
  /* count the number of path delimiters */
  for( sp = spx; *sp; sp++ )
    {
      if(( *sp == PATH_DELIMITER1 ) || ( *sp == PATH_DELIMITER2 ))
        {
          delimiter = *sp;  /* store delimiter */
          cnt++;
        }
    }
  
  /* switch on number of delimiters */
  switch( cnt )
    {
    case 0 :
      {
        
#ifdef ORIGINAL_CODE_REMOVED
        
        /* surely this code can't be right.....
	   it ensures that the info directory is *always* "./info",
	   and the filename is (incorrectly) set as the full pathname   */
        
        sprintf( path, "%s%s%s",
                getcwd( buf, sizeof( buf )), OPSYS_SEP, DEFAULT_PATH );
        strcpy( user, DEFAULT_USER );
        strcpy( filename, str );
        
#else
        strcpy( user, DEFAULT_USER );
        
        /* find the end of the directory part, if any, within the path */
        for( sp = &str[strlen( str )]; sp != str; ) {
          
          /* decrement now, so we're looking at real data instead of 
             just the '\0' */
          sp--;
          
          /* check for directory delimiter */
          if( *sp == OPSYS_SEP[0] ) {
            
            /* found, split apart the file and directory names */
            strcpy( filename, ++sp );
            sp--;  *sp = '\0';
            strcpy( buf, str );
            *sp = OPSYS_SEP[0];
            
            /* but, what if the directory is simply "/" ? */
            if( strlen( buf ) == 0) strcpy( buf, "/" );
            
            sprintf( path, "%s%s%s", buf, OPSYS_SEP, 
                    DEFAULT_PATH );
            return( INFO_SUCCESS );
          }
          
        }
        
        /* if here, then the input str contains no directory part, so *now* 
           use cwd */
        sprintf( path, "%s%s%s",
                getcwd( buf, sizeof( buf )), OPSYS_SEP, DEFAULT_PATH );
        strcpy( filename, str );
        
#endif
        
        return( INFO_SUCCESS );
      }
    case 1 :
      {
        sp = FetchNextWord( str, delimiter, path );
        strcpy( user, DEFAULT_USER );
        strcpy( filename, sp );
        
        return( INFO_SUCCESS );
      }
    case 2 :
      {
        sp = strchr( str, delimiter );
        
        sp = FetchNextWord( str, delimiter, path );
        sp = FetchNextWord( sp, delimiter, user );
        FetchNextWord( sp, delimiter, filename );
        
        return( INFO_SUCCESS );
      }
      default :
        
        return( INFO_FAIL );
    }
}


/***************************************************************************
  Name: InfoCoverFileName
  
  Purpose: This function generates ARC/INFO coverage INFO file names
  from a cover name.
  
  Parameters:
  inname ( char * ) Input coverage name
  type ( char ) Input coverage file type
  outname ( char * ) Returned coverage file name
  
  Outputs: Function - ( short int ) Returned function status and
  output coverage INFO file name.
  
  History: 11/92  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int InfoCoverFileName( inname, type, outname )
     
     char *inname;
     short int type;
     char *outname;
     
{
  char buf[PATH_SIZE];
  char *ext;
  char *sp;
  
  if( sp = strrchr( inname, OPSYS_SEP[0] ))
    {
      strncpy( buf, inname, ( inname - sp ));
      buf[( inname - sp ) + 1] = '\0';
    }
  else
    buf[0] = '\0';
  
  switch( type )
    {
    case INFO_COVER_PAT :
      ext = ".PAT";
      break;
    case INFO_COVER_AAT :
      ext = ".AAT";
      break;
    case INFO_COVER_NAT :
      ext = ".NAT";
      break;
    case INFO_COVER_BND :
      ext = ".BND";
      break;
    case INFO_COVER_TIC :
      ext = ".TIC";
      break;
    case INFO_COVER_ACODE :
      ext = ".ACODE";
      break;
    case INFO_COVER_ADD :
      ext = ".ADD";
      break;
    case INFO_COVER_TRN :
      ext = ".TRN";
      break;
      default :
        return( INFO_FAIL );
    }
  
  sprintf( outname, "%sinfo!arc!%s%s", buf, inname, ext );
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: InfoTempName
  
  Purpose: This function generates a temporary INFO file name for
  the input INFO directory.
  
  Parameters:
  infodir ( char * ) Input INFO directory
  outname ( char * ) Returned temporary file name
  
  Outputs:
  Function - ( short int ) Returned function status and 
  temporary INFO file name.  On failure, the contents of
  outname is unaltered.
  
  History: 11/92  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int InfoTempName( infodir, outname )
     
     char *infodir;
     char *outname;
     
{
  short int i;
  char buf[PATH_SIZE];
  
  for( i = 0; i < INFO_MAX_DIRSIZE; i++ )
    {
      sprintf( buf, "%s!TEMP%04ld", infodir, i );
      if( ! InfoFileExists( buf ))
        {
          strcpy( outname, buf );
          return( INFO_SUCCESS );
        }
    }
  
  return( INFO_FAIL );
}
