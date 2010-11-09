/* inforead.c
   ------------------------------------------------------------------------
   Reads an INFO data file and writes an ASCII file of it.
   This is just for testing infoxlib stuff.  See also infowrite.
   
   Written: 08/16/94 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   ------------------------------------------------------------------------ */

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

int main(argc, argv)
     int    argc;
     char   *argv[];
{
  int       numargs, done, ok, status;
  long      i, pos;
  FILE      *ioout;
  char      ofile[80];
  char      temp80[80];
  char      orec[(INFO_MAXLEN*2)+1], itemval[INFO_MAXLEN+1];
  long      numitemstodo, recnum, itemnum;
  
  /* These are for the INFO file. */
  
  char           *InfoFilePath;
  InfoFile       *FilePtr;
  TemplateList   *Template;
  long int        reclen, numrecs;
  short int       numitems, numredefitems, doredefined;
  short int       startcol, itemtype, itemilen, itemolen, itemnumdec;
  char            itemname[INFO_NAMELEN];
  
  /* Set program details. */
  
  char *progname="INFOREAD";
  char *progargs="<info_file> <out_file>";
  char *progver= "1.2 January 17, 1995";
  int  authorid=FALSE;
  
  
  /* Check number of arguments. */
  
  numargs = argc - 1;
  if (numargs < 2 || numargs > 2)
    {
      usage(progname,progver,progargs,authorid);
      exit(0);
    }

  
  /* Fetch arguments, check validity. */

  InfoFilePath=argv[1];
  strcpy(ofile,argv[2]);

    
  /* Open the output file. */
  
  if ((ioout = fopen(ofile, "w")) == NULL)
    {
      printf("%s: Could not open output file \"%s\".\n",progname,ofile);
      exit(0);
    }
#ifdef DEBUGGING
  printf("%s: Output ascii file opened ok.\n", progname);
#endif

  
  /* Open the input file for reading. */
  
  if((FilePtr = InfoOpenFile(InfoFilePath, InfoREAD)) == (InfoFile *) NULL)
    {
      printf("%s: Could not open input file \"%s\".\n",progname,InfoFilePath);
      exit(0);
    }
#ifdef DEBUGGING
  printf("%s: Input info file opened ok.\n", progname);
#endif

  
  /* Fetch header terms. */

  numrecs=InfoNumberRecords(FilePtr);
  numitems=InfoNumberItems(FilePtr);
  reclen=InfoRecordLength(FilePtr);
#ifdef DEBUGGING
  printf("%d\n",reclen);
  printf("%d\n",numrecs);
  printf("%d\n",numitems);
#endif
  status=getnumredef(FilePtr,&numredefitems);
  if (status != INFO_OK)
    {
      infoderror(progname,status,FilePtr,ioout);
      exit(0);
    }
#ifdef DEBUGGING
  printf("%d\n",numredefitems);
  printf("%s: Fetch header terms ok.  See above.\n", progname);
#endif

  
  /* Fetch item defs. */
  
  if ((Template = getitems(FilePtr, numredefitems)) == (TemplateList *) NULL)
    {
      infoderror(progname,INFO_READ_TEMPLATE,FilePtr,ioout);
      exit(0);
    }
#ifdef DEBUGGING
  printf("%s: Fetch item template ok.\n", progname);
#endif
  
  
  /* Establish the number of items we're interested in.
     We don't want redefined items for this program! */
  
  doredefined=FALSE;
  numitemstodo=numitems;
  
  
  /* Write header terms. */

  fprintf(ioout,"%d\n",reclen);
  fprintf(ioout,"%d\n",numrecs);
  fprintf(ioout,"%d\n",numitems);
  fprintf(ioout,"%d\n",numredefitems);
#ifdef DEBUGGING
  printf("%s: Write header terms ok.\n", progname);
#endif
  
  
  /* Write item definitions. */

  for (itemnum=0; itemnum < numitems; itemnum++)
    fprintf(ioout,"%4d %-16s %4d %4d %2d %2d\n",
            Template[itemnum].ItemPosition,
            Template[itemnum].ItemName,
            Template[itemnum].ItemWidth,
            Template[itemnum].OutputWidth,
            Template[itemnum].ItemType,
            Template[itemnum].NumberDecimals);
#ifdef DEBUGGING
  printf("%s: Write item definitions ok.\n", progname);
#endif
  
  
  /* List the data section. */
  
  for(recnum = 1; recnum <= FilePtr->NumberRecords; recnum++)
    {
      
      /* Read a record from the INFO file. */
      
      if(!InfoReadRecord(FilePtr, recnum))
        {
          infoderror(progname,INFO_READ_RECORD,FilePtr,ioout);
          exit(0);
        }
      
      /* Convert record to ascii format. */
      
      status=recinfo2asc(FilePtr,Template,orec,numredefitems,doredefined);
      if (status != INFO_OK)
        {
          infoderror(progname,status,FilePtr,ioout);
          exit(0);
        }
      
      /* Print it. */
      
      fprintf(ioout,"%s\n",orec);
      
    }
#ifdef DEBUGGING
  printf("%s: Write data section ok.\n", progname);
#endif
  
  
  /* Close files. */
  
  if(!InfoCloseFile(FilePtr))
    {
      printf("%s: Could not close input file \"%s\".\n", progname,InfoFilePath);
      exit(0);
    }
  if (fclose(ioout) != 0)
    {
      printf("%s: Could not close output file \"%s\".\n",progname,ofile);
      exit(0);
    }
  
  
  /* All done. */
  
  fflush(stdout);
  return 1;
}
