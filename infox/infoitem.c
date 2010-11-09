/* @(#)infoitem.c	1.21 2/26/94 16:48:52 */
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
  Name: infoitem.c
  
  Description: Contains infolib item functions.  The following
  functions are contained in this module:
  
  InfoGetItemDef
  InfoItemDefFree
  InfoItemExists
  InfoDefineItem
  InfoDecode
  InfoEncode
  InfoItemPosition
  InfoItemWidth
  InfoItemType
  InfoItemNumberDecimals
  InfoItemOutputWidth
  
  History: 12/91  Todd Stellhorn  - original coding
  10/92  Todd Stellhorn  - InfoDecode updated
  
  10/94  Randy Deardorff - My own peculiar includes.
  
  12/94 Randy Deardorff - Changed the way the functions figure out
  whether they are looking at an 8.3 or pre-8.3 workspace.  Added
  file.h and calls to access system function.  This applies to
  InfoDefineItem only.
  
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
#include INFOSWAP_H
#undef DEBUGGING

extern dirtype_t dirtype;

/***************************************************************************
  Name: InfoGetItemDef
  
  Purpose: This function returns a pointer to an INFO item definition
  structure InfoItemDef based on an input INFO file pointer and an
  item name.  Regular, redefined and the pseudo item $RECNO are
  supported.  An attempt is made to test if the InfoFile pointer is
  valid by testing for NULL.  NULL is returned if an error occurs
  while reading the NIT file, if the item does not exist or if a
  memory allocation error occurs.  On success, a valid InfoItemDef
  pointer is returned.
  
  Parameters:
  ifp ( InfoFile * ) INFO file pointer
  itemname ( char * ) INFO item name
  
  Outputs:
  Function - ( InfoItemDef * ) Returned pointer to INFO item
  structure on success or NULL if item is not found or on error.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

InfoItemDef *InfoGetItemDef( ifp, itemname )
     
     InfoFile *ifp;
     char *itemname;
     
{
  long int      nitpos = 0;
  NitRecord     nitrec;
  InfoItemDef  *idp = ( InfoItemDef * ) NULL;
  
  if( ifp == ( InfoFile * ) NULL )
    return( idp );
  
  /* test for record number pseudo item */
  
  if( ! stricmp( itemname, INFO_RECNO ))
    {
      if(( idp = ( InfoItemDef * )malloc( sizeof( InfoItemDef ))) == NULL )
        return( idp );
      
      idp->ItemPosition = -1;
      idp->ItemWidth = -1;
      idp->ItemType = INFO_RECNO_TYPE;
      idp->NumberDecimals = 0;
      idp->OutputWidth = 5;
      strcpy( idp->ItemName, itemname );
      
      return( idp );  /* return here for $recno test */
    }
  
  if( ! NitGetRecord( ifp->NitFilePath, itemname, &nitrec, &nitpos ))
    return( idp );
  
  if(( idp = ( InfoItemDef * )malloc( sizeof( InfoItemDef ))) == NULL )
    return( idp );
  
  idp->ItemPosition = nitrec.ItemPosition;
  idp->ItemWidth = nitrec.ItemWidth;
  idp->ItemType = nitrec.ItemType;
  idp->NumberDecimals = nitrec.NumberDecimals;
  idp->OutputWidth = nitrec.OutputWidth;
  strcpy( idp->ItemName, itemname );
  
  return( idp );
}


/***************************************************************************
  Name: InfoItemDefFree
  
  Purpose: This function returns item definition memory to free memory.
  
  Parameters: ipd ( InfoItemDef * ) Item definition pointer
  
  Outputs: Function - ( void )
  
  History: 11/92  Todd Stellhorn  - adpated from ItemDefFree
  
  ***************************************************************************/

void InfoItemDefFree( idp )
     
     InfoItemDef *idp;
     
{
  if( idp != ( InfoItemDef * ) NULL )
    free( idp );
}


/***************************************************************************
  Name: InfoItemExists
  
  Purpose: This function tests for the existence of an item on an open
  INFO file.  An attempt is made to test if the InfoFile pointer is
  valid by testing for NULL.  A successful return indicates that the
  item exists in the NIT for the input InfoFile pointer.  An error
  occurs if the item does not exist or an error occurred while reading
  the NIT file.
  
  Parameters:
  ifp ( InfoFile * ) INFO file pointer
  itenname ( char * ) INFO item name
  
  Outputs:
  Function - ( short int ) Returned existence status or error
  INFO_FAIL == item does not exist or error
  INFO_SUCCESS == item exists
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int InfoItemExists( ifp, itemname )
     
     InfoFile *ifp;
     char *itemname;
     
{
  long int      nitpos = 0;
  NitRecord     nitrec;
  
  if( ifp == ( InfoFile * ) NULL )
    return( INFO_FAIL );
  
  if( ! stricmp( itemname, INFO_RECNO ))
    return( INFO_SUCCESS );
  
  if( ! NitGetRecord( ifp->NitFilePath, itemname, &nitrec, &nitpos ))
    return( INFO_FAIL );
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: InfoDefineItem
  
  Purpose: This function adds an item to an existing *empty* INFO file
  template.  The INFO file is assumed to be closed while the new item
  is being added.  Function status is returned.
  
  Parameters:
  name ( char * ) Input INFO file pathname
  itemname ( char * ) Input item name
  width ( short int ) Input item width
  output ( short int ) Input item output width
  type ( short int ) Input item type
  numdec ( short int ) Input item number of decimals
  
  Outputs: Function - ( short int ) Function status
  
  History: 11/92  Todd Stellhorn  - original coding
  
  12/94 Randy Deardorff - Fixed 8.3 v pre-8.3 logic
  
  01/95 Randy Deardorff - Added reference to MacroPROMOTE to determine
  whether to promote type to long.
  
  ***************************************************************************/

short int InfoDefineItem( name, itemname, width, output, type, numdec )
     
     char *name;
     char *itemname;
     short int width;
     short int output;
#if MacroPROMOTE
     long int type;        /* type set to long int due to #define promotion */
#else
     int type;
#endif
     short int numdec;
     
{
  char          infodir[PATH_SIZE];
  char          username[USERNAME_SIZE];
  char          filename[INTERNALNAME_SIZE];
  char          dirpath[PATH_SIZE];
  char          extname[EXTNAME_SIZE];
  char          nitpath[PATH_SIZE];
  char          item[ITEMNAME_SIZE];
  char          tmpitemname[ITEMNAME_SIZE];
  char          tmpalter[ITEMNAME_SIZE];
  FILE         *nitfp = ( FILE * ) NULL;
  DirRecord     dirrec;
  long int      dirpos = 0;
  NitRecord     nitrec;
  long int      nitpos = 0;
  long int      itempos = 1;
  
#ifdef DEBUGGING
  printf("InfoDefineItem:      name=%s\n",name);
  printf("                 itemname=%s\n",itemname);
  printf("                    width=%d\n",width);
  printf("                   output=%d\n",output);
  printf("                     type=%d\n",type);
  printf("                   numdec=%d\n",numdec);
#endif
  
  
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
  
  if( dirrec.NumberRecords != 0 )
    return( INFO_FAIL );
  
  TerminateString( dirrec.ExternalName, sizeof( dirrec.ExternalName ),
                  extname );
  
  MAKENIT (nitpath, infodir, extname);
  
  /* open nit and search for itemname -- also update record position
     nitpos */
  
  if(( nitfp = OpenFile( nitpath, InfoREAD )) == ( FILE * ) NULL )
    return( INFO_FAIL );
  
  strcpy( item, itemname );
  StringtoUpper( item );
  
  while( NitReadRecord( nitfp, &nitrec, nitpos ))
    {
      TerminateString( nitrec.ItemName, sizeof ( tmpitemname ),
                      tmpitemname );
      TerminateString( nitrec.AlternateName, sizeof( tmpalter ),
                      tmpalter );
      
      if(( stricmp( tmpitemname, item ) == 0 ) ||
         ( stricmp( tmpalter, item ) == 0 ))
        {
          CloseFile( nitfp );
          return( INFO_FAIL );
        }
      
      nitpos++;
    }  
  
  if( ! CloseFile( nitfp ))
    return( INFO_FAIL );
  
  /* save last item's item position */
  if( nitpos )  /* if one or more records read */
    itempos = nitrec.ItemPosition + nitrec.ItemWidth;
  
  /* init nit record */
  memset( &nitrec, ' ', sizeof( nitrec ));
  
  ExpandString( item, 16 );
  strncpy( nitrec.ItemName, item, 16 );
  
  nitrec.ItemWidth           = width;
  nitrec.KeyType             = -1;     /* default to no key */
  nitrec.ItemPosition        = itempos;
  nitrec.ProtectionLevel     = 4;
  nitrec.Filler1             = -1;
  nitrec.OutputWidth         = output;
  
  if(( type == INFO_FLOATING_TYPE ) || ( type == INFO_NUMBER_TYPE ))
    nitrec.NumberDecimals = numdec;
  else
    nitrec.NumberDecimals = -1;
  
  nitrec.ItemType            = type;
  nitrec.ReadOnlyFlag        = 0;         /* default to off */
  nitrec.KeyLevel            = -1;
  nitrec.IndexNumber         = -1;
  nitrec.OccuranceCount      = -1;
  nitrec.Filler2             = -1;
  /* AlternateName init to ' ' above */
  /* Filler3 init to ' ' above */
  nitrec.ConcordLevel        = -1;
  nitrec.AlternateRule       = -1;
  /* Filler4 init to ' ' above */
  nitrec.ConcordCase         = -1;
  nitrec.ConcordNumbers      = -1;
  nitrec.ItemNumber          = nitpos + 1;
  /* Filler5 init to ' ' above */
  
  /* write record to nit */
  if( ! NitWriteRecord( nitpath, &nitrec, nitpos ))
    return( INFO_FAIL );
  
  /* update dr9 file */
  dirrec.NumberItems = dirrec.NumberItems + 1;
  dirrec.RecordLength = dirrec.RecordLength + width;
  
  /* record lengths must be even number of bytes */
  if( dirrec.RecordLength % 2 )
    dirrec.RecordLength++;
  
  if( ! DirWriteRecord( dirpath, &dirrec, dirpos ))
    return( INFO_FAIL );
  
  return( INFO_SUCCESS );
}


/***************************************************************************
  Name: InfoDecode
  
  Purpose: This function decodes and returns string or double values
  from the IOBuffer pointed to by ifp based on an item definition
  pointed to by idp.  The decode process extracts the values from the
  IOBuffer and places it into the output buffers pointed to by str or
  dptr.  All supported numeric values are returned via dptr.  The
  following INFO data types are supported:
  
  D - date (INFO_DATE_TYPE)
  C - character (INFO_CHARACTER_TYPE)
  I - integer (INFO_INTEGER_TYPE)
  N - numeric (INFO_NUMERIC_TYPE)
  B - binary (2 and 4 byte) (INFO_BINARY_TYPE)
  F - float (4 and 8 byte) (INFO_FLOAT_TYPE)
  $RECNO - record number pseudo item (INFO_RECNO_TYPE)
  
  An error occurs if an attempt is made to decode an unsupported data
  type.  On error, the contents of the buffers pointed to by str and
  dptr are undefined.
  
  Parameters:
  ifp ( InfoFile * ) INFO file pointer
  idp ( InfoItemDef * ) INFO item definition
  str ( char * ) Decoded string
  dptr ( double * ) Decoded double
  
  Outputs:
  Function - ( short int )  Returned decode status
  INFO_FAIL == decode error
  INFO_SUCCCESS == decode ok
  
  Decoded values (string or double) are returned.
  
  History: 12/91 Todd Stellhorn - original coding
  
  10/92 Todd Stellhorn - fixed binary 4 decode loop value
  
  11/92 Todd Stellhorn - added byte swapping
  
  08/93 Xinguo Wei (Univ. of Wash.) - added byte swapping for floats
  as needed for some Ultrix systems

  1/95 Randy Deardorff - Made 4 byte type B items use long if on
  BORLAND_C.
  
  ***************************************************************************/

short int InfoDecode( ifp, idp, str, dptr )
     
     InfoFile *ifp;
     InfoItemDef *idp;
     char *str;
     double *dptr;
     
{
  switch ( idp->ItemType )
    {
    case INFO_DATE_TYPE :
    case INFO_CHARACTER_TYPE :
      {
        memcpy( str, &ifp->IOBuffer[idp->ItemPosition - 1], 
               idp->ItemWidth );
        str[idp->ItemWidth] = '\0';
        
        return( INFO_SUCCESS );
      }
      
    case INFO_INTEGER_TYPE :
    case INFO_NUMBER_TYPE :
      {
        char        tmpstr[TMPRECORD_SIZE];
        
        memcpy( tmpstr,
               &ifp->IOBuffer[idp->ItemPosition - 1], idp->ItemWidth );
        tmpstr[idp->ItemWidth] = '\0';
        
        *dptr = atof( tmpstr );
        
        return( INFO_SUCCESS );
      }
      
      /* for binary, float and double types move each byte individually */
      
    case INFO_BINARY_TYPE :
      {
        switch ( idp->ItemWidth )
          {
          case 2 :
            {
              short int     tmp;
              register int  k;
              char *src = &ifp->IOBuffer[idp->ItemPosition - 1];
              char *dest = ( char * )&tmp;
              
              for( k = 0; k < 2; k++ )
                dest[k] = src[k];
              
              INFO_BYTESWAP( &tmp, sizeof( tmp ), 1 );
              
              *dptr = ( double ) tmp;
              
              return( INFO_SUCCESS );
            }
          case 4 :
            {
#if BORLAND_C
              long     tmp;
#else
              int      tmp;
#endif
              register int  k;
              char *src = &ifp->IOBuffer[idp->ItemPosition - 1];
              char *dest = ( char * )&tmp;
              
              for( k = 0; k < 4; k++ )
                dest[k] = src[k];
              
              INFO_BYTESWAP( &tmp, sizeof( tmp ), 1 );
              
              *dptr = ( double ) tmp;
              
              return( INFO_SUCCESS );
            }
            default :
              
              return( INFO_FAIL );
          }
      }
      
    case INFO_FLOATING_TYPE :
      {
        switch ( idp->ItemWidth )
          {
          case 4 :
            {
              float         tmp;
              register int  k;
              char *src = &ifp->IOBuffer[idp->ItemPosition - 1];
              char *dest = ( char * )&tmp;
              
              for( k = 0; k < 4; k++ )
                dest[k] = src[k];
              
              INFO_BYTESWAP( &tmp, sizeof( tmp ), 1 );
              
              *dptr = ( double ) tmp;
              
              return( INFO_SUCCESS );
            }
          case 8 :
            {
              double        tmp;
              register int  k;
              char *src = &ifp->IOBuffer[idp->ItemPosition - 1];
              char *dest = ( char * )&tmp;
              
              for( k = 0; k < 8; k++ )
                dest[k] = src[k];
              
              INFO_BYTESWAP( &tmp, sizeof( tmp ), 1 );
              
              *dptr = tmp;
              
              return( INFO_SUCCESS );
            }
            default :
              
              return( INFO_FAIL );
          }
      }
      
    case INFO_LEADFILL_TYPE     :    /* unsupported data types */
    case INFO_PACKED_TYPE       :
    case INFO_ZEROFILL_TYPE     :
    case INFO_OVERPUNCH_TYPE    :
    case INFO_TRAILINGSIGN_TYPE :
      
      return( INFO_FAIL );
      
    case INFO_RECNO_TYPE        :    /* record number pseudo item */
      {
        *dptr = ( double ) ifp->CurrentRecord;
        
        return( INFO_SUCCESS );
      }
      
      default :
        
        return( INFO_FAIL );
    }
}


/***************************************************************************
  Name: InfoEncode
  
  Purpose: This function encodes string or double values into the
  IOBuffer pointed to by ifp based on an item definition pointed to by
  idp.  The encode process updates the IOBuffer by inserting the input
  values.  All supported numeric values are accepted by the routine
  using dval.  The following INFO data types are supported:
  
  D - date (INFO_DATE_TYPE)
  C - character (INFO_CHARACTER_TYPE)
  I - integer (INFO_INTEGER_TYPE)
  N - numeric (INFO_NUMERIC_TYPE)
  B - binary (2 and 4 byte) (INFO_BINARY_TYPE)
  F - float (4 and 8 byte) (INFO_FLOAT_TYPE)
  
  An error occurs if an attempt is made to encode an unsupported data
  type or the pseudo item $RECNO.  This function does not return an
  error is data value truncation occurs.
  
  Parameters:
  ifp ( InfoFile * ) INFO file pointer
  idp ( InfoItemDef * ) INFO item definition
  str ( char * ) Encode string pointer
  dval ( double  ) Encode double value
  
  Outputs:
  Function - ( short int )  Returned encode status
  INFO_FAIL == encode error
  INFO_SUCCESS == encode ok
  
  IOBuffer is updated with encoded values.  On error, the contents of
  the IOBuffer is undefined.
  
  History: 12/91 Todd Stellhorn - original coding
  
  11/92 Todd Stellhorn - added byte swapping
  
  08/93 Xinguo Wei (Univ. of Wash.) - added byte swapping for floats
  as needed for some Ultrix systems

  1/95 Randy Deardorff - Made 4 byte type B items use long if on
  BORLAND_C.
  
  6/97 Rabdy Deardorff - Use double rather than long int format when
  converting type I values.  This corrects overflow on values greater
  than 2147483647.
  
  ***************************************************************************/

short int InfoEncode( ifp, idp, str, dval )
     
     InfoFile *ifp;
     InfoItemDef *idp;
     char *str;
     double dval;
     
{
  switch ( idp->ItemType )
    {
    case INFO_DATE_TYPE :
    case INFO_CHARACTER_TYPE :
      {
        FillStringBuffer( str, idp->ItemPosition, idp->ItemWidth, 
                         ifp->IOBuffer);
        
        return( INFO_SUCCESS );
      }
      
    case INFO_INTEGER_TYPE :
      {
        char        tmp[TMPSTRING_SIZE];
        char        fmt[TMPSTRING_SIZE];
        
        /* sprintf( fmt, "%%%dld", idp->ItemWidth ); */
        /* sprintf( tmp, fmt, ( long int ) dval ); */
        
        sprintf( fmt, "%%%d.0lf", idp->ItemWidth );
        sprintf( tmp, fmt, dval );
        
        FillStringBuffer( tmp, idp->ItemPosition, idp->ItemWidth, 
                         ifp->IOBuffer);
        
        return( INFO_SUCCESS );
      }
      
    case INFO_NUMBER_TYPE :
      {
        char        tmp[TMPSTRING_SIZE];
        char        fmt[TMPSTRING_SIZE];
        
        sprintf( fmt, "%%%d.%dlf", idp->ItemWidth, idp->NumberDecimals 
                );
        sprintf( tmp, fmt, dval );
        
        FillStringBuffer( tmp, idp->ItemPosition, idp->ItemWidth, 
                         ifp->IOBuffer);
        
        return( INFO_SUCCESS );
      }
      
      /* for binary, float and double types move each byte individually */
      
    case INFO_BINARY_TYPE :
      {
        switch ( idp->ItemWidth )
          {
          case 2 :
            {
              short int      tmp = ( short int )dval;
              register int   k;
              char *dest = &ifp->IOBuffer[idp->ItemPosition - 1];
              char *src = ( char * )&tmp;
              
              INFO_BYTESWAP( &tmp, sizeof( tmp ), 1 );
              
              for( k = 0; k < 2; k++ )
                dest[k] = src[k];
              
              return( INFO_SUCCESS );
            }
          case 4 :
            {
#if BORLAND_C
              long     tmp = ( long int )dval;
#else
              int       tmp = ( long int )dval;
#endif
              register int   k;
              char *dest = &ifp->IOBuffer[idp->ItemPosition - 1];
              char *src = ( char * )&tmp;
              
              INFO_BYTESWAP( &tmp, sizeof( tmp ), 1 );
              
              for( k = 0; k < 4; k++ )
                dest[k] = src[k];
              
              return( INFO_SUCCESS );
            }
            default :
              
              return( INFO_FAIL );
          }
      }
      
    case INFO_FLOATING_TYPE :
      {
        switch ( idp->ItemWidth )
          {
          case 4 :
            {
              float          tmp = ( float )dval;
              register int   k;
              char *dest = &ifp->IOBuffer[idp->ItemPosition - 1];
              char *src = ( char * )&tmp;
              
              INFO_BYTESWAP( &tmp, sizeof( tmp ), 1 );
              
              for( k = 0; k < 4; k++ )
                dest[k] = src[k];
              
              return( INFO_SUCCESS );
            }
          case 8 :
            {
              double         tmp = dval;
              register int   k;
              char *dest = &ifp->IOBuffer[idp->ItemPosition - 1];
              char *src = ( char * )&tmp;
              
              INFO_BYTESWAP( &tmp, sizeof( tmp ), 1 );
              
              for( k = 0; k < 8; k++ )
                dest[k] = src[k];
              
              return( INFO_SUCCESS );
            }
            default :
              
              return( INFO_FAIL );
          }
        
      }
      
    case INFO_LEADFILL_TYPE     :    /* unsupported data types */
    case INFO_PACKED_TYPE       :
    case INFO_ZEROFILL_TYPE     :
    case INFO_OVERPUNCH_TYPE    :
    case INFO_TRAILINGSIGN_TYPE :
    case INFO_RECNO_TYPE        :    /* $recno cannot be encoded */
      default :
        
        return( INFO_FAIL );
    }
}


/***************************************************************************
  Name: InfoItemPosition
  
  Purpose: This function returns the item's position.
  
  Parameters: ifp ( InfoFile * ) INFO file pointer 
  
  Outputs: Function - ( short int )  Returned item position
  
  History: 11/92  Todd Stellhorn  - original coding 
  
  ***************************************************************************/ 

short int InfoItemPosition( idp ) 
     
     InfoItemDef *idp;
     
{ 
  return( idp->ItemPosition ); 
}


/***************************************************************************
  Name: InfoItemWidth
  
  Purpose: This function returns the item's width.
  
  Parameters: ifp ( InfoFile * ) INFO file pointer
  
  Outputs: Function - ( short int )  Returned item width
  
  History: 11/92  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int InfoItemWidth( idp )
     
     InfoItemDef *idp;
     
{
  return( idp->ItemWidth );
}


/***************************************************************************
  Name: InfoItemType
  
  Purpose: This function returns the item's type.
  
  Parameters: ifp ( InfoFile * ) INFO file pointer
  
  Outputs: Function - ( short int )  Returned item type
  
  History: 11/92  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int InfoItemType( idp )
     
     InfoItemDef *idp;
     
{
  return( idp->ItemType );
}


/***************************************************************************
  Name: InfoItemNumberDecimals
  
  Purpose: This function returns the item's number decimals.
  
  Parameters: ifp ( InfoFile * ) INFO file pointer
  
  Outputs: Function - ( short int )  Returned item number decimals
  
  History: 11/92  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int InfoItemNumberDecimals( idp )
     
     InfoItemDef *idp;
     
{
  return( idp->NumberDecimals );
}


/***************************************************************************
  Name: InfoItemOutputWidth
  
  Purpose: This function returns the item's output width.
  
  Parameters: ifp ( InfoFile * ) INFO file pointer
  
  Outputs: Function - ( short int )  Returned item output width
  
  History: 11/92  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int InfoItemOutputWidth( idp )
     
     InfoItemDef *idp;
     
{
  return( idp->OutputWidth );
}
