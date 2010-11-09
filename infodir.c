/* infodir.c
   ------------------------------------------------------------------------
   Lists the contents of an INFO directory.
   
   Written: 09/07/94 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   12/15/94 -- Updates for 8.3 v pre 8.3 workspaces.  Got rid of
   troublesome cwd call.  Not needed anyway.
   
   12/28/94 Added in-line help.
   
   12/29/94 Fixed bugs in optional arg handling that prevented you
   from listing all files of a certain type, eg, infodir df.  Also
   fixed bug that caused the program to fail if you specified a single
   file, eg infodir cover.pat.  Also fixed bug that caused files that
   did not match filespec to be listed if they matched filetype.  Now
   they must match both.

   1/20/95 Changes to work on DOS.

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

int isinfodir();
int help();
int writehelp();

int main(argc, argv)
     int    argc;
     char   *argv[];
{
  short int     numargs, done, curcol, nomatch, match, pos1, pos2, jlen;
  long int      i, pos;
  short int     doallfiles, dowidelist, ignorecase, numitems, reclen;
  short int     typetolist, gousage, newdirtype;
  char          formatstring[80];
  short int     filespecarg = 1, typetolistarg = 2;
  
  char          filespec[128], temp[128], infodirpath[128], filetype[3];
  char          definfopath[128];
  
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
  
  char *ftypelist="DF|PG|RP|SF|IF";
  
  
  /* Set program details. */
  
  char *progname="INFODIR";
  char *progargs="{-lc} {files} {DF|PG|RP|SF|IF}";
  char *progver= "5.7 July 13, 1998";
  int  authorid=TRUE;
  
  
  /* Fetch arguments, check validity. */
  
  numargs = argc - 1;

  
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
  
  /* Fetch and set any dash option flags. */
  
  dowidelist=FALSE;
  ignorecase=TRUE;
  gousage=FALSE;
  if (numargs >= 1)
    {
      strcpy(temp,argv[1]);
      if (temp[0] == '-')
        {
          filespecarg++;
          typetolistarg++;
          for (i = 1; i < strlen(temp); i++)
            switch (temp[i])
              {
              case 'l':
                dowidelist=TRUE;
                break;
              case 'c':
                ignorecase=FALSE;
                break;
              default:
                gousage=TRUE;
                break;
              }
        }
    }
  if (gousage)
    {
      usage(progname,progver,progargs,authorid);
      return FALSE;
    }
  
  
  /* Make sure not too many args. */
  
  if (numargs > typetolistarg)
    {
      usage(progname,progver,progargs,authorid);
      return FALSE;
    }


  /* Get the filespec arg. */

  if (numargs >= filespecarg)
    {
      strcpy(filespec,argv[filespecarg]);
      if (strcmp(filespec,"#") == 0 || strcmp(filespec,"@") == 0)
        {
          doallfiles = TRUE;
          filespec[0]='\0';
        }
      else
        {
          doallfiles = FALSE;
          stredt(filespec,'#','*');
          stredt(filespec,'%','*');
          stredt(filespec,'@','*');
          strunquote(filespec);
        }
    }
  else
    {
      doallfiles = TRUE;
      filespec[0]='\0';
      typetolistarg--;
    }


  /* Check if filespec is really a filetype, and adjust accordingly. */

  typetolist = -1;
  strcpy(temp,filespec);
  strupr(temp);
  i=strkeyword(ftypelist,temp,'|',"left");
  if (i >= 1)
    {
      /* Filespec is really a filetype.  */
      
      strcpy(filespec,"*");
      typetolist=i-1;
    }
  else
    {
      /* Filespec is not a file type.  Check for filetype arg. */
      
      if (numargs >= typetolistarg)
        {
          strcpy(temp,argv[typetolistarg]);
          strupr(temp);
          typetolist=strkeyword(ftypelist,temp,'|',"left");
          if (typetolist <= 0)
            {
              usage(progname,progver,progargs,authorid);
              return FALSE;
            }
          else
            {
              doallfiles = FALSE;
              typetolist--;
            }
        }
    }
  
  
  /* Set path to INFO directory, and or cleanup filespec. */
  
  definfopath[0]='\0';
  strcat(definfopath,"info");
  infodirpath[0]='\0';
  if (filespec[0] == '\0')
    {
      /* No path or filespec arg.  Assume current directory. */
      
      strcpy(infodirpath,definfopath);
    }
  else
    {
      if (strfind(filespec,OPSYS_SEP,0) < 0)
        {

#ifdef DEBUGGING
          printf("No separator.  A file or wildcard in current directory.\n");
#endif
          
          /* If filespec has no star, it could be either an info file
             name, or the workspace name.  Have to check. */
          
          if (strfind(filespec,"*",0) < 0)
            {
              
#ifdef DEBUGGING
              printf("No star, could be workspace name.  Checking...\n");
#endif
              syscwd(temp);
              strcat(temp,OPSYS_SEP);
              strcat(temp,filespec);
              strcat(temp,OPSYS_SEP);
              strcat(temp,"info");
              if (isinfodir(temp))
                {
                  strcpy(infodirpath,temp);
                  filespec[0]='\0';
                  doallfiles=TRUE;
                }
              else
                {
                  syscwd(infodirpath);
                  strcat(infodirpath,OPSYS_SEP);
                  strcat(infodirpath,"info");
                }
            }
          else
            {
              
#ifdef DEBUGGING
              printf("Has star, can't be the workspace name.\n");
#endif
              strcpy(infodirpath,definfopath);
            }
        }
      else
        {
          
#ifdef DEBUGGING
          printf("At least one path separator.\n");
#endif
          
          /* Get separate directory and entry name.  Note that we
             remove OPSYS_SEP"info:arc:" from directory. */
          
          sprintf(temp,"%s%s",OPSYS_SEP,"info:arc:");
          pos1 = strfind(filespec,temp,0);
          sprintf(temp,"%s%s",OPSYS_SEP,"info!arc!");
          pos2 = strfind(filespec,temp,0);
          jlen = strlen(filespec);
          if (pos1 >= 0)
            {
              strsubstr(filespec,infodirpath,0,pos1);
              strsubstr(filespec,temp,pos1+10,jlen-pos1-10+1);
              strcpy(filespec,temp);
            }
          else
            if (pos2 >= 0)
              {
                strsubstr(filespec,infodirpath,0,pos2);
                strsubstr(filespec,temp,pos2+10,jlen-pos2-10+1);
                strcpy(filespec,temp);
              }
            else
              {
                strcpy(temp,filespec);
                strentry(temp,filespec);
                strdir(temp,infodirpath);
              }
          
          /* If filespec has no star, it could be either an info file
             name, or the workspace name.  Have to check. */
          
          if (strfind(filespec,"*",0) < 0)
            {
              strcpy(temp,infodirpath);
              strcat(temp,OPSYS_SEP);
              strcat(temp,filespec);
              strcat(temp,OPSYS_SEP);
              strcat(temp,"info");
              if (isinfodir(temp))
                {
                  strcpy(infodirpath,temp);
                  filespec[0]='\0';
                  doallfiles=TRUE;
                }
            }
          
          /* Append OPSYS_SEP"info" if not supplied. */
          
          sprintf(temp,"%s%s",OPSYS_SEP,"info");
          if (strfind(infodirpath,temp,0) < 0 )
            {
              strcat(infodirpath,OPSYS_SEP);
              strcat(infodirpath,"info");
            }
        }
    }
  
  
  /* Ignore case? */
  
  if (ignorecase)
    strlwr(filespec);

  
  /* Args ok.  Continue. */
  
#ifdef DEBUGGING
  printf("infodirpath=%s\n",infodirpath);
  printf("   filespec=%s\n",filespec);
  printf(" dowidelist=%d\n",dowidelist);
  printf(" ignorecase=%d\n",ignorecase);
  printf(" typetolist=%d\n",typetolist);
#endif
  
  
  /* Open the INFO dir file. */
  
#ifdef DEBUGGING
  printf(" infodirpath=%s\n",infodirpath);
#endif
  
  sprintf(dirfile,"%s%s%s%s",infodirpath,OPSYS_SEP,"arc",DIR_EXT);
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
      newdirtype=FALSE;
    }
  else
    {
      sprintf(dirfile,"%s%s%s",infodirpath,OPSYS_SEP,"arc.dir");
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
          newdirtype=TRUE;
        }
      else
        {
          printf("%s: Could not open INFO directory file.\n",progname);
          return FALSE;
        }
    }
  
#ifdef DEBUGGING
  printf("OpenFile ok.\n");
#endif
  
  
  /* Set up. */
  
  pos=0;
  done=FALSE;
  curcol=1;
  nomatch=TRUE;
  if (doallfiles)
    match=TRUE;
  else
    match=FALSE;
  if (dowidelist)
    {
      printf(" Type Name                            Internal Name     Records");
      printf(" Length External\n");
      if (sizeof(long)==(sizeof(dirrec.NumberRecords)))
        if (newdirtype)
          strcpy(formatstring,"  %s  %-32s  %-9s %12ld %6d    %c%c\n");
        else
          strcpy(formatstring,"  %s  %-32s  %-9s %13ld %6d    %c%c\n");
      else
        if (newdirtype)
          strcpy(formatstring,"  %s  %-32s  %-9s %12d %6d    %c%c\n");
        else
          strcpy(formatstring,"  %s  %-32s  %-9s %13d %6d    %c%c\n");
    }
  
  
  /* List files until eof or error. */
  
  while(!done)
    if(!DirReadRecord(dirfp,&dirrec,pos))
      done=TRUE;
    else
      {
        if (!dirrec.EmptyFlag)
          {

            /* Get internal (user level) file name. */
            
            strsubstr(dirrec.InternalName,filename,0,INTERNALNAME_SIZE-1);
            strtrim(filename,"both",' ');

            
            /* Check if this file matches our search criteria. */

            if (!doallfiles)
              if (ignorecase)
                {
                  strcpy(temp,filename);
                  strlwr(temp);
                  match=strmatch(temp,filespec);
                }
              else
                match=strmatch(filename,filespec);
            
            if (match)
              if (typetolist >= 0)
                match=(typetolist==dirrec.FileType);
            
            
            /* Print the filename to screen. */
            
            if (match)
              {
                if (dowidelist)
                  
                  /* Single column listing pretty much like INFO does. */
                  
                  {
                    
                    /* Get external name.  We have to add the three letter
                       tag for DAT or SRC.  Type DF files are DAT.  All others
                       are SRC. */
                    
                    strsubstr(dirrec.ExternalName,extname,0,EXTNAME_SIZE-1);
                    strtrim(extname,"both",' ');
                    if (dirrec.FileType == 0)
                      strcat(extname,"DAT");
                    else
                      strcat(extname,"SRC");
                    
                    /* Get the file type from our list. */
                    
                    strext(ftypelist,dirrec.FileType+1,filetype,'|');
                    
                    /* Print information for this file. */
                    
                    printf(formatstring,
                           filetype,
                           filename,
                           extname,
                           dirrec.NumberRecords,
                           dirrec.RecordLength,
                           dirrec.ExternalFlag[0],
                           dirrec.ExternalFlag[1]);
                    
                  }
                else
                  
                  /* Two column listing. */
                  
                  {
                    if (curcol == 1)
                      {
                        printf("%-32s",filename);
                        curcol++;
                      }
                    else
                      {
                        printf(" %-32s\n",filename);
                        curcol=1;
                      }
                  }
                nomatch=FALSE;
              }
          }
        pos++;
      }
  
  
  /* Wrap up. */
  
  if (!dowidelist && curcol == 2)
    putchar('\n');

  if (nomatch)
    if (doallfiles)
      printf("No files.\n");
    else
      printf("No match.\n");
  
  
  /* Close the INFO directory file. */
  
  if(!CloseFile(dirfp))
    {
      printf("%s: Could not close INFO directory file: %s.\n",progname,dirfile);
      return FALSE;
    }
  
  return TRUE;
}


int isinfodir(infodirpath)
     char *infodirpath;
{
  char          infodir[128];
  char          dirfile[128];
  char          username[USERNAME_SIZE];
  char          filename[INTERNALNAME_SIZE];
  FILE          *dirfp = (FILE *) NULL;
  short int     retval;
  
#ifdef DEBUGGING
  printf("  Checking if %s is an info directory...\n",infodirpath);
#endif
  
  sprintf(dirfile,"%s%s%s%s",infodirpath,OPSYS_SEP,"arc",DIR_EXT);

#ifdef DEBUGGING
  printf("    Trying %s...\n",dirfile);
#endif

  if (sysexist(dirfile))
    {
      if((dirfp = OpenFile(dirfile,InfoREAD)) != (FILE *) NULL)
        {
          CloseFile(dirfp);
          retval = TRUE;
        }
      else
        retval = FALSE;
    }
  else
    {
      sprintf(dirfile,"%s%s%s",infodirpath,OPSYS_SEP,"arc.dir");

#ifdef DEBUGGING
      printf("    Trying %s...\n",dirfile);
#endif

      if (sysexist(dirfile))
        {
          if((dirfp = OpenFile(dirfile,InfoREAD)) != (FILE *) NULL)
            {
              CloseFile(dirfp);
              retval = TRUE;
            }
          else
            retval = FALSE;
        }
      else
        retval = FALSE;
    }
  
#ifdef DEBUGGING
  if (retval)
    printf("    Yes, %s opened ok.\n",dirfile);
  else
    printf("    No.\n");
#endif
  
  return retval;
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
  fprintf(io,"Usage: infodir {-lc} {files} {DF|PG|RP|SF|IF}\n");
  fprintf(io,"\n");
  fprintf(io,"Lists the contents of an INFO directory without starting an INFO\n");
  fprintf(io,"session.  Runs from UNIX or DOS level.  Can also be run from ARC via\n");
  fprintf(io,"&system.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Arguments:\n");
  fprintf(io,"\n");
  fprintf(io,"{-lc} -l Long listing.  Default is shorter two column list.\n");
  fprintf(io,"      -c Consider case.  Default is ignore case.\n");
  fprintf(io,"\n");
  fprintf(io,"{files} -- Files to list.  See notes.\n");
  fprintf(io,"\n");
  fprintf(io,"{DF|PG|RP|SF|IF} -- Type of INFO files to list.  Default is all types.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Notes:\n");
  fprintf(io,"\n");
  fprintf(io,"If no arguments are given, all INFO files in the current workspace's\n");
  fprintf(io,"INFO directory are listed in two column format.\n");
  fprintf(io,"\n");
  fprintf(io,"You can specify files to list either by full name, or by partial name\n");
  fprintf(io,"with wildcard characters.  On UNIX, the customary wildcard character,\n");
  fprintf(io,"'*', works only if you enclose the file specification in single\n");
  fprintf(io,"quotes.  For example:\n");
  fprintf(io,"\n");
  fprintf(io,"    infodir '*.pat'\n");
  fprintf(io,"\n");
  fprintf(io,"If you prefer, you can substitute '@' or '%%' for the wildcard.  In\n");
  fprintf(io,"this case, you don't need to quote the file specification.\n");
  fprintf(io,"\n");
  fprintf(io,"You can specify another directory to list, giving either a full path,\n");
  fprintf(io,"or a relative path.  For example:\n");
  fprintf(io,"\n");
  fprintf(io,"    infodir /usr/workspace/cover.pat\n");
  fprintf(io,"    infodir workspace/cover.pat\n");
  fprintf(io,"\n");
  fprintf(io,"You can use /info:arc: if you want, but you don't need to.  For example,\n");
  fprintf(io,"the following are equivalent:\n");
  fprintf(io,"\n");
  fprintf(io,"    infodir /usr/workspace/info:arc:cover.pat\n");
  fprintf(io,"    infodir /usr/workspace/cover.pat\n");
  fprintf(io,"\n");
  fprintf(io,"See also infodel, infolist, infoitems, and infolook.\n");
  /* @ end help */
  return 1;
}
