/* strmatch.c            A function in misclib.
   ------------------------------------------------------------------------
   int strmatch(filename,filespec)
   char *filename;
   char *filespec;
   
   This funtion checks the filename against the filespec and returns true
   if it matches and false if it doesn't match or an error occured.

   The filename and the filespec must be 128 characters or less.

   Written: 09/08/94 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

#define TRUE 1
#define FALSE 0

int strmatch(filename,filespec)
     char *filename;
     char *filespec;
{
  short int  match, pos, pos1, pos2, numstars, fslen, fnlen;
  short int  match1, match2, match3;
  char       chunk1[129], chunk2[129], chunk3[129];
  
  /* Set up. */
  
  match=FALSE;
  numstars=0;
  fslen=strlen(filespec);
  fnlen=strlen(filename);

  
  /* See how many stars. */

  for (pos1 = 0; pos1 < fslen; pos1++)
    if (filespec[pos1]=='*')
      numstars++;

  
  /* Check match according to how many stars. */

  switch (numstars)
    {
    case 0 :
      if (strcmp(filename,filespec) == 0)
        match=TRUE;
      break;
      
    case 1 :
      if (strcmp(filespec,"*") == 0)
        match=TRUE;
      else
        {
          pos1=strfind(filespec,"*",0);
          if (pos1==0)
            {
              strsubstr(filespec,chunk2,pos1+1,fslen-1);
              pos=strrfind(filename,chunk2,-1);
              if (pos >= 0)
                if (pos + strlen(chunk2) == fnlen)
                  match=TRUE;
            }
          else
            if (pos1==fslen-1)
              {
                strsubstr(filespec,chunk1,0,fslen-1);
                if (strfind(filename,chunk1,0) == 0)
                  match=TRUE;
              }
            else
              {
                strsubstr(filespec,chunk2,pos1+1,fslen-pos1-1);
                strsubstr(filespec,chunk1,0,fslen-strlen(chunk2)-1);
                if (strfind(filename,chunk1,0) == 0)
                  {
                    pos=strrfind(filename,chunk2,-1);
                    if (pos >=0)
                      if (pos + strlen(chunk2) == fnlen)
                        match=TRUE;
                  }
              }
        }
      break;
      
    case 2 :
      pos1=strfind(filespec,"*",0);
      pos2=strrfind(filespec,"*",-1);
      strsubstr(filespec,chunk1,0,pos1);
      strsubstr(filespec,chunk2,pos1+1,pos2-pos1-1);
      strsubstr(filespec,chunk3,pos2+1,fslen-pos2-1);
      
      if (chunk1[0] == '\0')
        match1=TRUE;
      else
        match1=(strfind(filename,chunk1,0) == 0);
      
      if (chunk2[0] == '\0')
        match2=TRUE;
      else
        match2=(strfind(filename,chunk2,pos1) >= 0);
      
      if (chunk3[0] == '\0')
        match3=TRUE;
      else
        match3=(strrfind(filename,chunk3,-1) + strlen(chunk3) == fnlen);
      
      match=(match1 && match2 && match3);
      break;
      
    default:
      /* More than two stars is always no match. */
      match=FALSE;
      break;
    }
  return match;
}
