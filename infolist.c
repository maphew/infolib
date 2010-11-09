/* infolist.c
   --------------------------------------------------------------------------
   Lists the data section of a INFO file to the standard output device.
   The data section will be listed to stdout.  This is usually the screen.
   You can use redirection to capture the data to an output file.
   
   Written: 08/16/94 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (All changes by the author unless otherwise noted)

   12/28/94 Added in-line help.

   --------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include INFOLIB_H
#include INFO_H

#define TRUE 1
#define FALSE 0
#undef DEBUGGING

int help();
int writehelp();

int main(argc, argv)
     int    argc;
     char   *argv[];
{
  int       numargs, done, ok, status;
  FILE      *ioout;
  char      temp[128];
  char      orec[(INFO_MAXLEN*2)+1], itemval[INFO_MAXLEN+1];
  long      recnum, itemnum;
  short int doredefined;
  
  
  /* These are for the INFO file. */
  
  char           *InfoFilePath;
  InfoFile       *FilePtr;
  TemplateList   *Template;
  long int        reclen, numrecs;
  short int       numitems, numredefitems;
  short int       startcol, itemtype, itemilen, itemolen, itemnumdec;
  char            itemname[INFO_NAMELEN];
  
  
  /* Set program details. */
  
  char *progname="INFOLIST";
  char *progargs="<info_file> {REDEFINED}";
  char *progver= "5.7 July 13, 1998";
  int  authorid=TRUE;
  
  
  /* Check number of arguments. */
  
  numargs = argc - 1;
  if (numargs < 1 || numargs > 2)
    {
      usage(progname,progver,progargs,authorid);
      return 0;
    }
   

  /* Check if help requested. */
  
  strcpy(temp,argv[1]);
  strupr(temp);
  if (strcmp(temp,"-HELP")==0)
    {
      help(progname,FALSE);
      return FALSE;
    }
  
  
  /* Fetch arguments, check validity. */
  
  InfoFilePath=argv[1];
  
  if (numargs >= 2)
    {
      strcpy(temp,argv[2]);
      if (strcmp(temp,"#") == 0 || strcmp(temp,"@") == 0)
        strcpy(temp,"NOREDEFINED");
      else
        {
          strupr(temp);
          if (temp[0]=='R')
            strcpy(temp,"REDEFINED");
          if (temp[0]=='N')
            strcpy(temp,"NOREDEFINED");
          if (strcmp(temp,"REDEFINED") && strcmp(temp,"NOREDEFINED"))
            {
              usage(progname,progver,progargs,authorid);
              return 0;
            }
        }
    }
  else
    strcpy(temp,"NOREDEFINED");
  doredefined = !strcmp(temp,"REDEFINED");

  
  /* Open the input file for reading. */
  
  if((FilePtr = InfoOpenFile(InfoFilePath,InfoREAD)) == (InfoFile *) NULL)
    {
      printf("%s: Could not open input file \"%s\".\n",progname,InfoFilePath);
      return 0;
    }
  
  
  /* Fetch header terms. */
  
  numrecs=InfoNumberRecords(FilePtr);
  numitems=InfoNumberItems(FilePtr);
  reclen=InfoRecordLength(FilePtr);
  status=getnumredef(FilePtr,&numredefitems);
  if (status != INFO_OK)
    {
      infoderror(progname,status,FilePtr,ioout);
      return 0;
    }
  
  /* Fetch item defs. */
  
  if((Template = getitems(FilePtr,numredefitems)) ==
     (TemplateList *) NULL)
    {
      infoderror(progname,INFO_READ_TEMPLATE,FilePtr,ioout);
      return 0;
    }
  
  
  /* List the data section. */
  
  for(recnum = 1; recnum <= FilePtr->NumberRecords; recnum++)
    {
      
      /* Read a record from the INFO file. */
      
      if(!InfoReadRecord(FilePtr,recnum))
        {
          infoderror(progname,INFO_READ_RECORD,FilePtr,ioout);
          return 0;
        }
      
      /* Convert record to ascii format. */
      
      status=recinfo2asc(FilePtr,Template,orec,numredefitems,doredefined);
      if (status != INFO_OK)
        {
          infoderror(progname,status,FilePtr,ioout);
          return 0;
        }
      
      /* Print it. */
      
      printf("%s\n",orec);
      
    }

  
  /* Close files. */
  
  if(!InfoCloseFile(FilePtr))
    {
      printf("%s: Could not close input file \"%s\".\n", progname,InfoFilePath);
      return 0;
    }
  
  
  /* All done. */
  
  fflush(stdout);
  return 1;
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
  fprintf(io,"Usage: infolist <info_file> {REDEFINED}\n");
  fprintf(io,"\n");
  fprintf(io,"Lists the data section of an INFO data file.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Arguments:\n");
  fprintf(io,"\n");
  fprintf(io,"<info_file> -- The INFO data file to examine.\n");
  fprintf(io,"\n");
  fprintf(io,"{REDEFINED} -- Specifies whether redefined items will be listed.  May\n");
  fprintf(io,"  be abbreviated to {R|N}.  The default is NOREDEFINED.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Notes:\n");
  fprintf(io,"\n");
  fprintf(io,"The data section will be listed to stdout.  This is usually the screen.\n");
  fprintf(io,"You can use redirection to capture the data to an output file.\n");
  fprintf(io,"\n");
  fprintf(io,"You can specify the full path to an INFO file by using the standard\n");
  fprintf(io,"/path/path/path/info:arc:info_file syntax.\n");
  fprintf(io,"\n");
  fprintf(io,"You can skip arguments and accept the default with \"#\" or \"@\".\n");
  fprintf(io,"\n");
  fprintf(io,"See also infoitems, infolook, infodir, and infodel.\n");
  /* @ end help */
  return 1;
}
