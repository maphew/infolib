/* infodel.c
   ------------------------------------------------------------------------
   Deletes INFO files, with wildcarding.
   
   If the INFO file is an external file, only the
   internal NIT and DAT files are deleted -- the external file is not
   removed.
   
   Written: 01/04/95 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   02/03/95 Added "%" and "#" alternates for "*" besides "@".
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include INFOLIB_H
#include INFO_H
#include INFODEFS_H

#define TRUE 1
#define FALSE 0
#undef DEBUGGING

int help();
int writehelp();

int main(argc, argv)
     int    argc;
     char   *argv[];
{
  short int     numargs, done, match, morefiles;
  long int      i, pos;
  
  char          filespec[128], temp[128], infodirpath[128];
  char          definfopath[128];
  
  char          infodir[128];
  char          dirfile[128];
  char          username[USERNAME_SIZE];
  char          filename[INTERNALNAME_SIZE];
  char          tfilename[INTERNALNAME_SIZE];
  char          dirpath[128];
  char          extname[EXTNAME_SIZE+2];
  char          extpath[128];
  char          nitpath[128];
  char          tmppath[128];
  DirRecord     dirrec;
  char          internalname[INTERNALNAME_SIZE];
  FILE          *dirfp = (FILE *) NULL;
  
  
  /* Set program details. */
  
  char *progname="INFODEL";
  char *progargs="<files>";
  char *progver= "5.7 July 13, 1998";
  int  authorid=TRUE;
  
  
  /* Fetch arguments, check validity. */
  
  numargs = argc - 1;
#ifdef DEBUGGING
  printf("%d args\n",numargs);
#endif
  
  
  /* Check if help requested. */
  
  if (numargs >= 1)
    {
      strcpy(temp,argv[1]);
      strupr(temp);
      if (strcmp(temp,"-HELP")==0)
        {
          help(progname,FALSE);
          return FALSE;
        }
    }
  
  
  /* Make sure not too many or too few args. */
  
  if (numargs > 1 || numargs < 1 )
    {
      usage(progname,progver,progargs,authorid);
      return FALSE;
    }
  

  /* Get the filespec arg. */
  
  strcpy(filespec,argv[1]);
  stredt(filespec,'#','*');
  stredt(filespec,'%','*');
  stredt(filespec,'@','*');
  strunquote(filespec);
  
  
  /* Set path to INFO directory, and or cleanup filespec. */
  
  definfopath[0]='\0';
  strcat(definfopath,"info:arc:");
  infodirpath[0]='\0';
  strcpy(infodirpath,definfopath);

  
  /* Ignore case? */
  
  strlwr(filespec);

  
  /* Args ok.  Continue. */
  
#ifdef DEBUGGING
  printf("infopath=%s\n",infodirpath);
  printf("filespec=%s\n",filespec);
#endif
  
  
  /* Open the INFO dir file. */
  
  if(!InfoParsePath(infodirpath,infodir,username,filename))
    {
      printf("%s: Could not parse INFO path: %s.\n",progname,infodirpath);
      return FALSE;
    }
  
  sprintf(dirfile,"%s%s%s%s",infodir,OPSYS_SEP,"arc",DIR_EXT);

#ifdef DEBUGGING
  printf("Trying %s\n",dirfile);
#endif
  
  if (sysexist(dirfile))
    {
      if((dirfp = OpenFile(dirfile,InfoREAD)) == (FILE *) NULL)
        {
          printf("%s: Could not open INFO directory file %s.\n",
                 progname,dirfile);
          return FALSE;
        }
    }
  else
    {
      sprintf(dirfile,"%s%s%s",infodir,OPSYS_SEP,"arc.dir");
      
#ifdef DEBUGGING
      printf("Trying %s\n",dirfile);
#endif
      
      if (sysexist(dirfile))
        {
          if((dirfp = OpenFile(dirfile,InfoREAD)) == (FILE *) NULL)
            {
              printf("%s: Could not open INFO directory file %s.\n",
                     progname,dirfile);
              return FALSE;
            }
        }
      else
        {
          printf("%s: Could not open INFO directory file.\n",progname);
          return FALSE;
        }
    }
  
#ifdef DEBUGGING
  printf("Info directory %s opened ok.\n",dirfile);
#endif
  
  
  /* Now that we know we can open the INFO directory file, close it. */

  if(!CloseFile(dirfp))
    {
      printf("%s: Could not close INFO directory file: %s.\n",progname,dirfile);
      return FALSE;
    }
  
  
  /* Delete matching files until done, eof, or error. */
  
  pos=0;
  morefiles=TRUE;
  while(morefiles)
    {
      
      /* Open the directory file. */
      
      if((dirfp = OpenFile(dirfile,InfoREAD)) == (FILE *) NULL)
        {
          printf("%s: Could not open INFO directory file %s.\n",
                 progname,dirfile);
          return FALSE;
        }

      /* Seek a matching file in the directory. */
      
      done=FALSE;
      while(!done)
        {
          
#ifdef DEBUGGING
          printf("Pos=%ld\n",pos);
#endif
          
          match=FALSE;
          if(!DirReadRecord(dirfp,&dirrec,pos))
            {
              done=TRUE;
              morefiles=FALSE;
            }
          else
            {
              if (!dirrec.EmptyFlag)
                {
                  
                  /* Get internal (user level) file name. */
                  
                  strsubstr(dirrec.InternalName,filename,0,INTERNALNAME_SIZE-1);
                  strtrim(filename,"both",' ');
                  
#ifdef DEBUGGING
                  printf("Considering %s...\n",filename);
#endif
                  
                  /* Check if this file matches our search criteria. */
                  
                  strcpy(temp,filename);
                  strlwr(temp);
                  match=strmatch(temp,filespec);

                  
                  /* Delete matching files. */
                  
                  if (match)
                    {

                      /* Temporarily close the directory. */
                      
                      if(!CloseFile(dirfp))
                        {
                          printf("%s: Error closing INFO directory file: %s.\n",
                                 progname,dirfile);
                          return FALSE;
                        }
                      
                      /* Delete the file. */
                      
                      if (!InfoDeleteFile(temp))
                        {
                          printf("%s: Could not delete INFO file \"%s\".\n",
                                 progname,temp);
                          return FALSE;
                        }
                      
                      /* re-open the directory file. */
                      
                      if((dirfp = OpenFile(dirfile,InfoREAD)) == (FILE *) NULL)
                        {
                          printf("%s: Error opening INFO directory file %s.\n",
                                 progname,dirfile);
                          return FALSE;
                        }
                      
                    }
                }
            }
          if (!match)
            pos++;
        }
      
      
      /* Close the INFO directory file. */
      
      if(!CloseFile(dirfp))
        {
          printf("%s: Could not close INFO directory file: %s.\n",
                 progname,dirfile);
          return FALSE;
        }
      
    }
  return TRUE;
}


/* Help function. */

int help (progname)
     char       *progname;
{
  
  FILE *io;
  char ftag[12], filename[64], cmd[128];
  
  
  /* Set filename. */
  
  if (!sysdate("ftag",ftag))
    {
      printf("%s: Could not generate temporary file name.\n",progname);
      return FALSE;
    }
  sprintf(filename,"xx%s",ftag);
  strlwr(filename);
  
  
  /* Open the file for writes. */
  
  if ((io = fopen(filename, "w")) == NULL)
    {
      printf("%s: Could open %s for writing.\n",progname,filename);
      return FALSE;
    }
  
  
  /* Write the help text to the file. */
  
  writehelp(io);  
  
  
  /* Close the file. */
  
  fclose(io);
  
  
  /* Display it, then delete it. */
  
#if ANY_UNIX
  sprintf(cmd,"more %s",filename);
#else
  sprintf(cmd,"page %s",filename);
#endif  
  system(cmd);
  sysdel(filename);
  
  
  /* Done. */
  
  return 1;
}

int writehelp (io)
     FILE *io;
{
  /* @ begin help */
  fprintf(io,"Usage: infodel <files>\n");
  fprintf(io,"\n");
  fprintf(io,"Deletes INFO files, with wildcarding.\n");
  fprintf(io," \n");
  fprintf(io,"If the INFO file is an external file, only the internal NIT and DAT\n");
  fprintf(io,"files are deleted -- the external file is not removed.\n");
  fprintf(io,"\n");
  fprintf(io,"Runs from UNIX or DOS level.  Can also be run from ARC via &system.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Arguments:\n");
  fprintf(io,"\n");
  fprintf(io,"<files> -- Files to delete.  See notes.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Notes:\n");
  fprintf(io,"\n");
  fprintf(io,"This program is NOT case sensitive for filenames.  It deletes files\n");
  fprintf(io,"regardless of case.\n");
  fprintf(io,"\n");
  fprintf(io,"You can specify files to list either by full name, or by partial name\n");
  fprintf(io,"with wildcard characters.  On UNIX, the customary wildcard character,\n");
  fprintf(io,"'*', works only if you enclose the file specification in single\n");
  fprintf(io,"quotes.  For example:\n");
  fprintf(io,"\n");
  fprintf(io,"    infodel 'junk.*'\n");
  fprintf(io,"\n");
  fprintf(io,"If you prefer, you can substitute '@' or '%%' for the wildcard.  In\n");
  fprintf(io,"this case, you don't need to quote the file specification.\n");
  fprintf(io,"\n");
  fprintf(io,"    infodel junk.@\n");
  fprintf(io,"\n");
  fprintf(io,"See also infodir, infolist, infoitems, and infolook.\n");
  /* @ end help */
  return 1;
}
