/* @(#)infostr.c	1.9 2/16/94 */
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
  Name: infostr.c
  
  Description: Contains infolib string processing functions.  The
  following functions are contained in this module:
  
  TerminateString
  StringtoLower
  StringtoUpper
  FillStringBuffer
  CompressString
  ExpandString
  CompareStrings
  FetchNextWord
  
  History: 12/91  Todd Stellhorn  - original coding
  
  10/94  Randy Deardorff - my own peculiar includes.
  
  01/95  Randy Deardorff - added stdio.h.  This is not needed
  unless you are also including rbdpaths.h.
  
  ***************************************************************************/

#include <stdio.h>
#include <rbdpaths.h>
#include INFOLIB_H
#include INFODEFS_H

/***************************************************************************
  Name: TerminateString
  
  Purpose: This function copies the values from the string buffer
  pointed to by src into the string buffer pointed to by dest.  The
  string buffer dest is NULL terminated on return.  The value of size
  is used to indicate the length of src.  Trailing spaces (' ') are
  truncated from dest.  The function is used to convert INFO's
  non-terminated FORTRAN strings into terminated C strings.
  
  Parameters:
  src ( char * ) String buffer
  size ( short int ) Maximum size of non-terminated
  string pointed to by src
  dest ( char * ) Return buffer for terminated string
  
  Outputs: Function - ( void ) Terminated string pointed to by dest.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

void TerminateString( src, size, dest )
     
     char *src;
     short int size;
     char *dest;
     
{
  register int     i;
  
  for( i = 0; i < size; i++)
    {
      if( src[i] == ' ' )
        break;
      dest[i] = src[i];
    }
  
  dest[i] = '\0';
}


/***************************************************************************
  Name: StringtoLower
  
  Purpose: This function translates the string pointed to by buf to
  lower case.
  
  Parameters: buf ( char * ) Updated string buffer
  
  Outputs: Function - ( void ) Updated string buffer pointed to by buf.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

void StringtoLower( buf )
     
     char *buf;
{
  while( *buf )
    {
      *buf = tolower( *buf );
      buf++;
    }
}


/***************************************************************************
  Name: StringtoUpper
  
  Purpose: This function translates the string pointed to by buf to
  upper case.
  
  Parameters: buf ( char * ) Updated string buffer
  
  Outputs: Function - ( void ) Updated string buffer pointed to by buf.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

void StringtoUpper( buf )
     
     char *buf;
     
{
  while( *buf )
    {
      *buf = toupper( *buf );
      buf++;
    }
}


/***************************************************************************
  Name: FillStringBuffer
  
  Purpose: This function places the values pointed to by inbuf into
  the buffer pointed to by outbuf.  The insertion position within
  outbuf is input via pos and the length of the inserted values is
  input via len.  If the length of inbuf is less than len, outbuf is
  padded with spaces (' ').  Function is used to insert a string of
  values into the IOBuffer.
  
  Parameters:
  inbuf ( char * ) String buffer
  pos ( short int ) Buffer string position
  len ( short int ) Buffer string length
  outbuf ( char * ) Updated string buffer
  
  Outputs:
  Function - ( void )  Updated string buffer pointed to by outbuf.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

void FillStringBuffer( inbuf, pos, len, outbuf )
     
     char *inbuf;
     short int pos;
     short int len;
     char *outbuf;
     
{
  register int        i;
  register int        k;
  short int           inlen = strlen( inbuf );
  
  for( i = 0, k = ( pos - 1 ); i < len; i++, k++ )
    {
      if( i < inlen )
        outbuf[k] = inbuf[i];
      else
        outbuf[k] = ' ';
    }
}


/***************************************************************************
  Name: CompressString
  
  Purpose: This function removes trailing spaces (' ') from the input
  string based on an input string width (size) and a string buffer
  pointed to by str.  The updated string buffer is returned.
  
  Parameters:
  str ( char * ) String buffer
  size ( short int ) String buffer width
  
  Outputs: Function - ( void ) Updated string pointed to by str.
  
  History: 12/91 Todd Stellhorn - original coding
  
  ***************************************************************************/

void CompressString( str, size )
     
     char *str;
     short int size;
     
{
  register int         i;
  for( i = ( size - 1 ); ( i > 0 ) && ( str[i] == ' ' ); --i );
  str[ i + 1 ] = '\0';
}


/***************************************************************************
  Name: ExpandString
  
  Purpose: This function pads the input string pointed to by str based
  on the value of size.  Trailing spaces are added to expand the
  string.  An updated string buffer is returned.
  
  Parameters:
  str ( char * ) String buffer
  size ( short int ) String buffer width
  
  Outputs: Function - ( void ) Updated string pointed to by str
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

void ExpandString( str, size )
     
     char *str;
     short int size;
     
{
  register int        i;
  for( i = ( strlen( str )); i < size; i++ )
    str[i] = ' ';
}


/***************************************************************************
  Name: CompareStrings
  
  Purpose: This function compares two input strings based on an input
  compare rule (CASE_SENSITIVE or NONCASE_SENSITIVE) and returns the
  string compare status.
  
  Parameters:
  str1 ( char * ) First string
  str2 ( char * ) Second sting
  cmprule ( short int ) Compare rule (CASE_SENSITIVE or 
  NONCASE_SENSITIVE)
  
  Outputs:
  Function - ( short int ) Returned compare status
  < 0 - str1 less than str2
  == 0 - str1 equal to str2
  > 0 - str1 greater than str2
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

short int CompareStrings( str1, str2, cmprule )
     
     char *str1;
     char *str2;
     short int cmprule;
     
{
  short int       cmp;
  
  while(( *str1 ) && ( *str2 ))
    {
      switch( cmprule )
        {
        case CASE_SENSITIVE     :
          
          cmp = *str1 - *str2;
          break;
          
        case NONCASE_SENSITIVE  :
          
          cmp = ( toupper( *str1 )) - ( toupper( *str2 ));
          break;
        }
      
      if( cmp )
        {
          return( cmp );
        }
      
      str1++;
      str2++;
      
    }
  
  cmp = ( strlen( str1 )) - ( strlen( str2 ));
  
  if( cmp )
    return( cmp );
  
  return( 0 );
}


/***************************************************************************
  Name: FetchNextWord
  
  Purpose: This function returns the next word based on an input
  string pointer and an input character delimiter.  A word is defined
  as one or more characters delimited by the value of the delimiter
  ch.  The position within the input string following the character
  delimiter is also returned.
  
  Parameters:
  sp ( char * ) Input string pointer
  ch ( char ) Input delimiter character
  nextword ( char * ) Input/Returned updated string
  
  Outputs:
  Function - ( char * ) Returned next input string word
  delimited by the value of ch.  Returned value is 
  placed in the character buffer nextword.  The string 
  position following the returned word is also returned.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  ***************************************************************************/

char *FetchNextWord( sp, ch, nextword )
     
     char *sp;
     char ch;
     char *nextword;
     
{
  while(( *sp != ch ) && ( *sp ))
    {
      *nextword = *sp;
      
      nextword++;
      sp++;
    }
  
  *nextword = '\0';
  
  return( ++sp );
}


/***************************************************************************
  Name: stricmp
  
  Purpose: This functions defines stricmp (case independent string
  compare) for systems which do not define it as part of their
  libraries.
  
  Parameters:
  s1 ( char * ) String one
  s2 ( char * ) String two
  
  Outputs:
  Function - ( int ) Returned string comparsion
  < 0 == s1 is less than s2
  = 0 == s1 is equal to s2
  > 0 == s1 is greater than s2
  
  History: 02/93  Todd Stellhorn  - original coding
  
  01/95 Randy Deardorff - Added conditional compilation of
  this function.  See infodefs.h.
  
  ***************************************************************************/

#if UseSTRICMP
int stricmp( s1, s2 )
     
     char *s1;
     char *s2;
     
{
  for( ; toupper( *s1 ) == toupper( *s2 ); ++s1, ++s2 )
    if( *s1 == '\0' )
      return( 0 );
  return(( *( unsigned char * )s1 ) - ( *( unsigned char * )s2 ));
}
#endif  /* UseSTRICMP */


/***************************************************************************
  AMLQuote
  
  This function places a quoted version of s2 into s1 using AML's
  quoting rules.  Quoted strings start and end with a single quote,
  and any internal single quotes are repeated twice, or doubled (not
  to be confused with double quotes). Some examples:
  
  this is a string   -> 'this is a string'
  embedded 'quote    -> 'embedded ''quote'
  'already ''quoted' -> '''already ''''quoted''' 
  
  Eric Weitzman, 2/94
  
  ***************************************************************************/

char * AMLQuote (s1, s2)
     char * s1;
     char * s2;
     
{
  char quote = '\'';
  char *start;
  
  start = s1;
  
  *s1++ = quote;
  while (*s2)
    {
      *s1 = *s2;
      if (*s1 == quote)
        {
          s1++;
          *s1 = quote;
        }
      s1++;
      s2++;
    }
  
  *s1++ = quote;
  *s1 = *s2;
  
  return (start);
}
