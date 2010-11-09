/* info2ascii.c
   ------------------------------------------------------------------------
   Creates a ascii data file and a structure file from an INFO data file.
   
   Written: 05/18/93 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (All changes are by the author unless otherwise noted)
   
   08/24/94 Converted to read INFO file directly rather than from e00
   file.  Now stand-alone C program; ARC/INFO not required.  Special
   arguments for when called by an AML so it can test for success.
   See info2ascii.aml.

   09/29/94 Allow double or single quotes as the quote character.
   
   12/28/94 Added in-line help.

   01/04/95 Increased size of dlm from 1 to 2.
   
   01/23/95 Elimiated fixed size arrays in favor of dynamically allocated
   structures.
   
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

int help();
int writehelp();

int main(argc, argv)
     int    argc;
     char   *argv[];
{
  short int numargs, status, done, i, ok, num, beg, len, gousage;
  FILE      *ioin, *ioout;
  char      ifile[128], datafile[128], defsfile[128], errfile[128];
  char      temp[128], dlm[2];
  char      arec[(INFO_MAXLEN*2)+1], orec[(INFO_MAXLEN*2)+1];
  short int doallitems, ffmt, defdfmt, defqopt, dfmt, dolen, quotechar;
  short int numitemstodo, itemfound, itemnum, numtodo;
  short int *itemnums = ( short int * ) NULL;
  short int *itembegs = ( short int * ) NULL;
  char      vbuff[INFO_MAXLEN], yyyy[5], yy[3], mm[3], dd[3];
  long int  rec;
  char      progargs[320];
  char      dattag[5], deftag[5];
  short int infofilearg=1, datafilearg=2, defsfilearg=3;
  short int ffmtarg=4, dfmtarg=5, qoptarg=6;
  short int itemarg=7;
  short int verbose=FALSE, silent=FALSE;
  
  
  /* These are for the INFO file. */
  
  char           *InfoFilePath;
  InfoFile       *FilePtr;
  TemplateList   *infoItems;
  long int        reclen, numrecs;
  short int       numitems, numredefitems, doredefined;
  short int       startcol, itemtype, itemilen, itemolen, itemnumdec;
  char            itemname[INFO_NAMELEN];


  /* Establish a look up list for output file formats. */
  
  char *ffmtlist="FIXED|LIST";
  
  
  /* Establish a look up list for date output formats. */
  
  char *dfmtlist="YYYYMMDD|MM/DD/YYYY|MM/DD/YY";
  
  
  /* Establish a look up list for quote argument. */
  
  char *quotelist="NOQUOTE|QUOTE|SINGLEQUOTE";
  
  
  /* Set program details. */
  
  char *unadargs="{-vs errorfile | -help }";
  char *progname="INFO2ASCII";
  char *progver= "5.7 July 13, 1998";
  int  authorid=TRUE;
  strcpy(progargs,"<info_file> {ascii_file} {structure_file}\n");
  strcat(progargs,
         "                  {FIXED | LIST | 'character' | 'ascii_code'}\n");
  strcat(progargs,"                  {YYYYMMDD | MM/DD/YYYY | MM/DD/YY}\n");
  strcat(progargs,"                  {NOQUOTE | QUOTE | SINGLEQUOTE}\n");
  strcat(progargs,"                  {REDEFINED | item...item}");
  
  
  /* Check number of arguments. */
  
  gousage=FALSE;
  numargs = argc - 1;
  
  if (numargs < 1)
    {
      usage(progname,progver,progargs,authorid);
      return FALSE;
    }
  
  
  /* Check if help requested. */
  
  strcpy(temp,argv[1]);
  strupr(temp);
  if (strcmp(temp,"-HELP")==0)
    {
      help(progname,FALSE);
      return FALSE;
    }
  
  
  /* Check for secret arguments used by calling programs only. */

  if (temp[0] == '-')
    {
      if (temp[1] == 'V')
        verbose=TRUE;
      else
        if (temp[1] == 'S')
          silent=TRUE;
        else
          {
            printf("%s\n",unadargs);
            usage(progname,progver,progargs,authorid);
            return FALSE;
          }
      if (verbose || silent)
        {
          if (numargs < 2)
            {
              printf("%s\n",unadargs);
              usage(progname,progver,progargs,authorid);
              return FALSE;
            }
          else
            {
              strcpy(errfile,argv[2]);
              for (i=1; i <= 2; i++)
                {
                  infofilearg++;
                  datafilearg++;
                  defsfilearg++;
                  ffmtarg++;
                  dfmtarg++;
                  qoptarg++;
                  itemarg++;
                }
            }
          if (numargs < infofilearg)
            {
              usage(progname,progver,progargs,authorid);
              return FALSE;
            }
        }
      
    }
  
  
  /* Fetch arguments, check validity. */
  
  /* Fetch input file name. */
  
  InfoFilePath=argv[infofilearg];
  
  /* Fetch file output format. */
  
  if (numargs >= ffmtarg)
    {
      strcpy(temp,argv[ffmtarg]);
      strunquote(temp);
      strupr(temp);
      if (strcmp(temp,"#") == 0 || strcmp(temp,"@") == 0)
        strcpy(temp,"FIXED");
    }
  else
    strcpy(temp,"FIXED");
  
  /* If the arg is a number, then assume this is an ascii code
     for a user-specified delimiter.  Convert to string.  */
  
  if (strisnum(temp))
    {
      if (strtype(temp) != -1)
        gousage=TRUE;
      else
        {
          i=atoi(temp);
          if (i < 1 || i > 255)
            {
              printf("%s: ascii_code must be from 1 to 255.\n",progname);
              return FALSE;
            }
          else
            {
              ffmt=100;        /* user-specified */
              dlm[0]=i;
              dlm[1]='\0';
              defdfmt=2;
              defqopt=2;
              strcpy(dattag,".txt");
              strcpy(deftag,".def");
            }
        }
    }
  else
    {
      
      /* If the unquoted length of the format is one, then assume this
         is a special user-specified delimiter.  Use it as is.  */
      
      if (strlen(temp)==1)
        {
          ffmt=100;        /* user-specified */
          dlm[0]=temp[0];
          dlm[1]='\0';
          defdfmt=2;
          defqopt=2;
          strcpy(dattag,".txt");
          strcpy(deftag,".def");
        }
      
      else
        {
          
          /* Ffmt is not a number and more than one character, ie, one of
             our keywords.  Make sure file format is one we know how to do,
             and set default date output format based on file format. */
          
          dlm[0]='\0';        /* no delimiter */
          strupr(temp);
          ffmt=strkeyword(ffmtlist,temp,'|',"left");
          switch (ffmt)
            {
            case -1:        /* ambiguous */
            case  0:        /* not found */
              gousage=TRUE;
              break;
              
            case 1:         /* FIXED */
              defdfmt=1;
              defqopt=1;
              strcpy(dattag,".dat");
              strcpy(deftag,".def");
              break;
              
            case 2:         /* LIST */
              defdfmt=2;
              defqopt=1;
              strcpy(dattag,".lis");
              strcpy(deftag,".def");
              break;
              
            }
        }
    }
  
  if (gousage)
    {
      usage(progname,progver,progargs,authorid);
      return FALSE;
    }


  /* Fetch output file names, if specified, or use defaults. */

  /* First get INFO filename, in case we need it to build default. */
  
  strcpy(vbuff,argv[infofilearg]);
  if (strfind(vbuff,"info:arc",0) >= 0)
    {
      strafter(vbuff,"info:arc",temp);
      strlshift(temp);
      strcpy(vbuff,temp);
    }
  else
    if (strfind(vbuff,"info!arc",0) >= 0)
      {
        strafter(vbuff,"info!arc",temp);
        strlshift(temp);
        strcpy(vbuff,temp);
      }

  
  /* Get (or build) data file name. */
  
  if (numargs >= datafilearg)
    {
      strcpy(temp,argv[datafilearg]);
      if (strcmp(temp,"#") == 0 || strcmp(temp,"@") == 0)
        temp[0]='\0';
      else
        strcpy(datafile,temp);
    }
  else
    temp[0]='\0';
  if (temp[0] == '\0')
    {
      strcpy(datafile,vbuff);
      strcat(datafile,dattag);
    }
  
  
  /* Get (or build) structure file name. */
  
  if (numargs >= defsfilearg)
    {
      strcpy(temp,argv[defsfilearg]);
      if (strcmp(temp,"#") == 0 || strcmp(temp,"@") == 0)
        temp[0]='\0';
      else
        strcpy(defsfile,temp);
    }
  else
    temp[0]='\0';
  if (temp[0] == '\0')
    {
      strcpy(defsfile,vbuff);
      strcat(defsfile,deftag);
    }
  
  
  /* Make sure structure and data files are different. */
  
  if (strcmp(defsfile,datafile) == 0)
    {
      printf("%s: Data and structure files must be different.\n",progname);
      return FALSE;
    }
  
  
  /* Fetch date output format. */
  
  /* If user gave a date format, use it, else set to null. */
  
  if (numargs >= dfmtarg)
    {
      strcpy(temp,argv[dfmtarg]);
      if (strcmp(temp,"#") == 0 || strcmp(temp,"@") == 0)
        *temp='\0';
      else
        strupr(temp);
    }
  else
    *temp='\0';
  
  /* If no date format, use default for file output type. */
  
  if (strnull(temp))
    strext(dfmtlist,defdfmt,temp,'|');
  
  /* Make sure date format is one we know how to do,
     And set special output width according to format. */
  
  dfmt=strkeyword(dfmtlist,temp,'|',"left");
  switch (dfmt)
    {
    case -1:        /* ambiguous */
    case  0:        /* not found */
      gousage=TRUE;
      break;
      
    case 1:         /* YYYYMMDD */
      dolen=8;
      break;
      
    case 2:         /* MM/DD/YYYY */
      dolen=10;
      break;
      
    case 3:         /* MM/DD/YY */
      dolen=8;
      break;
    }
  
  if (gousage)
    {
      usage(progname,progver,progargs,authorid);
      return FALSE;
    }
  
  
  /* Fetch quoted text option. */
  
  /* If user gave one, use it, else set to null. */
  
  if (numargs >= qoptarg)
    {
      strcpy(temp,argv[qoptarg]);
      if (strcmp(temp,"#") == 0 || strcmp(temp,"@") == 0)
        *temp='\0';
      else
        strupr(temp);
    }
  else
    *temp='\0';
  
  /* If no quote text option, use default for file output type. */
  
  if (strnull(temp))
    strext(quotelist,defqopt,temp,'|');
  
  /* Make sure it is one we know how to do. */
  
  i=strkeyword(quotelist,temp,'|',"left");
  switch (i)
    {
    case -1:        /* ambiguous */
    case  0:        /* not found */
      gousage=TRUE;
      break;
      
    case 1:         /* NOQUOTE */
      quotechar=-1;
      break;
      
    case 2:         /* QUOTE (double) */
      quotechar='"';
      break;
      
    case 3:         /* SINGLE QUOTE */
      quotechar='\'';
      break;
    }
  
  if (ffmt == 1 && quotechar > 0)
    {
      printf("%s: Can't quote fixed width file.\n",progname);
      return FALSE;
    }
  
  if (gousage)
    {
      usage(progname,progver,progargs,authorid);
      return FALSE;
    }
  
  
  /* Check for redefined option. */
  
  doredefined=FALSE;
  if (numargs >=  itemarg)
    {
      strcpy(temp,argv[itemarg]);
      strupr(temp);
      strunquote(temp);
      if (strcmp("REDEFINED",temp)==0 || strcmp("R",temp)==0)
        {
          doredefined=TRUE;
          if (numargs > itemarg)
            {
              usage(progname,progver,progargs,authorid);
              return FALSE;
            }
        }
    }
  
  
  /* Check for specified item list. */
  
  doallitems=TRUE;
  numitemstodo=0;
  if (!doredefined)
    if (numargs >= itemarg)
      {
        
        /* Items were specified.  Count them. */
        
        doallitems=FALSE;
        doredefined=TRUE;
        for (i=itemarg; i <= numargs; i++)
          numitemstodo++;
      }
  
  
  /* Print arguments */
  
#ifdef DEBUGGING
  printf("      Input file: %s\n",InfoFilePath);
  printf("       Data file: %s\n",datafile);
  printf("  Structure file: %s\n",defsfile);
  
  printf("   Output format: ");
  if (ffmt < 100)
    {
      strext(ffmtlist,ffmt,temp,'|');
      printf("%s\n",temp);
    }
  else
    printf("\"%s\"\n",dlm);
  
  printf("     Date format: ");
  strext(dfmtlist,dfmt,temp,'|');
  printf("%s\n",temp);
  
  printf("            Quote: ");
  if (quotechar > 0)
    printf("%c\n",quotechar);
  else
    printf("none\n");
  
  printf("       Item list: ");
  if (doallitems)
    if (doredefined)
      printf("All items, including redefined, will be processed.\n");
    else
      printf("All normal items will be processed.\n");
  else
    {
      for (i=itemarg; i <= numargs; i++)
        printf("%s ",argv[i]);
      putchar('\n');
    }
#endif
  
  
  /* Arguments check out.  Do the job. */
  
  if (verbose)
    printf("Creating %s and %s from INFO file %s...\n",
           datafile,defsfile,InfoFilePath);
  
  
  /* Open the input INFO data file for reading. */
  
  if((FilePtr = InfoOpenFile(InfoFilePath, InfoREAD)) == (InfoFile *) NULL)
    {
      printf("%s: Could not open INFO file \"%s\".\n",progname,InfoFilePath);
      return FALSE;
    }

  
  /* Fetch header terms. */
  
  numrecs=InfoNumberRecords(FilePtr);
  numitems=InfoNumberItems(FilePtr);
  reclen=InfoRecordLength(FilePtr);
  status=getnumredef(FilePtr,&numredefitems);
  if (status != INFO_OK)
    {
      infoderror(progname,status,FilePtr,ioout);
      return FALSE;
    }

  
  /* Fetch item defs. */
  
  if ((infoItems = getitems(FilePtr, numredefitems)) == (TemplateList *) NULL)
    {
      infoderror(progname,INFO_READ_TEMPLATE,FilePtr,ioout);
      return FALSE;
    }
  
  
  /* Set item numbers and start positions of items to be processed. */
  
  if (doredefined)
    numtodo=numitems+numredefitems;
  else
    numtodo=numitems;
  
  if ((itemnums = (short int *)malloc(sizeof(short int)*numtodo)) == NULL)
    {
      infoderror(progname,INFO_MALLOC,FilePtr,ioout);
      return FALSE;
    }
  
  if ((itembegs = (short int *)malloc(sizeof(short int)*numtodo)) == NULL)
    {
      infoderror(progname,INFO_MALLOC,FilePtr,ioout);
      return FALSE;
    }
  
  if (doallitems)
    {
      beg=1;
      for (num=0; num < numtodo; num++)
        {
          itemnums[num]=num;
          itembegs[num]=beg;
          beg=beg+infoItems[num].OutputWidth;
          numitemstodo++;
        }
    }
  else
    {
      itemfound=FALSE;
      for (i=0; i < numitemstodo; i++)
        {
          strcpy(temp,argv[i+itemarg]);
          strunquote(temp);
          itemfound=FALSE;
          beg=1;
          for (num=0; num < numtodo; num++)
            {
              if (strcmp(temp,infoItems[num].ItemName) == 0)
                {
                  itemnums[i]=num;
                  itembegs[i]=beg;
                  itemfound=TRUE;
                  break;
                }
              beg=beg+infoItems[num].OutputWidth;
            }
          if (!(itemfound))
            {
              printf("%s: Item \"%s\" not found in INFO file.\n",progname,temp);
              InfoCloseFile(FilePtr);
              return FALSE;
            }
        }
    }
  
  
  /* Open the structure file for writing. */
  
  if ((ioout = fopen(defsfile, "w")) == NULL)
    {
      printf("%s: Could not open output file \"%s\".\n",progname,defsfile);
      InfoCloseFile(FilePtr);
      return FALSE;
    }
  
  
  /* Write the structure to the structure file. */

  status=ascputdefs(ioout,itemnums,numitemstodo,dolen,numitems,infoItems);
  if (status != INFO_OK)
    {
      infoerror(progname,status,ioin,ioout);
      InfoCloseFile(FilePtr);
      return FALSE;
    }
  
  
  /* Close the structure file. */
  
  if (fclose(ioout) != 0)
    {
      printf("%s: Could not close output file \"%s\".\n",progname,defsfile);
      InfoCloseFile(FilePtr);
      return FALSE;
    }
  
  
  /* Open the datafile for writing. */
  
  if ((ioout = fopen(datafile, "w")) == NULL)
    {
      printf("%s: Could not open output file \"%s\".\n",progname,datafile);
      InfoCloseFile(FilePtr);
      return FALSE;
    }
  
  
  /* Loop through the data section record by record. */
  
  for (rec=1; rec <= FilePtr->NumberRecords; rec++)
    {
      
      /* Read a record from the INFO file. */
      
      if(!InfoReadRecord(FilePtr,rec))
        {
          infoderror(progname,INFO_READ_RECORD,FilePtr,ioout);
          return FALSE;
        }
      
      /* Convert the INFO data record to fixed width ASCII. */
      
      status=recinfo2asc(FilePtr,infoItems,arec,numredefitems,doredefined);
      if (status != INFO_OK)
        {
          infoderror(progname,status,FilePtr,ioout);
          return FALSE;
        }
      /* #ifdef DEBUGGING */
      /*       printf("ASCII: %s\n",arec); */
      /* #endif */

      
      /* Assemble the output record in the desired output format. */
      
      *orec='\0';
      for (itemnum=0; itemnum < numitemstodo; itemnum++)
        {
          num=itemnums[itemnum];
          beg=itembegs[itemnum];
          len=infoItems[num].OutputWidth;
          itemtype=infoItems[num].ItemType;
          
          /* Extract the current item value from the ascii record. */
          
          strsubstr(arec,vbuff,beg-1,len);
          
          /* Reformat date if necessary.  Dates come from the e00 file
             in YYYYMMDD (format 1) so we do nothing in that case. */
          
          if (itemtype==INFO_DATE_TYPE && dfmt != 1)
            {
              strsubstr(vbuff,yyyy,0,4);
              strsubstr(vbuff,yy,2,2);
              strsubstr(vbuff,mm,4,2);
              strsubstr(vbuff,dd,6,2);
              switch (dfmt)
                {
                case 2:
                  sprintf(vbuff,"%s/%s/%s",mm,dd,yyyy);
                  break;
                case 3:
                  sprintf(vbuff,"%s/%s/%s",mm,dd,yy);
                  break;
                }
            }
          
          /* Reformat item value according to the output file format. */
          
          switch (ffmt)
            {
            case 1:				/* FIXED */
              break;
              
            case 2:				/* LIST */
              if (itemnum != 0)
                strcat(orec," ");
              break;
              
            case 100:                         /* USER-SPECIFIED */
              strtrim(vbuff,"both",' ');
              if (itemnum != 0)
                strcat(orec,dlm);
              break;
              
            }
          
          /* Quote text items? */
          
          if (itemtype==INFO_CHARACTER_TYPE && quotechar > 0)
            {
              strinsert(vbuff,quotechar,0);
              strinsert(vbuff,quotechar,strlen(vbuff));
            }
          
          
          /* Append the current item value to the output record. */
          
          strcat(orec,vbuff);
          
          /* Next item value. */
          
        }
      
      /* Write output record to file. */
      
      fprintf(ioout,"%s\n",orec);
      
      /* #ifdef DEBUGGING */
      /*       printf("%s\n",orec); */
      /* #endif */
      
    }
  
  
  /* Close files. */
  
  if(!InfoCloseFile(FilePtr))
    {
      printf("%s: Could not close input file \"%s\".\n", progname,InfoFilePath);
      return FALSE;
    }
  if (fclose(ioout) != 0)
    {
      printf("%s: Could not close output file \"%s\".\n",progname,datafile);
      return FALSE;
    }
  
  
  /* Success.  If called by AML, remove error flag file. */
  
  if (verbose || silent)
    sysdel(errfile);
  
  
  /* Done. */
  
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
  fprintf(io,"Usage: INFO2ASCII <info_file> {ascii_file} {structure_file}\n");
  fprintf(io,"                  {FIXED | LIST | 'character' | 'ascii_code'}\n");
  fprintf(io,"                  {YYYYMMDD | MM/DD/YYYY | MM/DD/YY}\n");
  fprintf(io,"                  {NOQUOTE | QUOTE | SINGLEQUOTE}\n");
  fprintf(io,"                  {REDEFINED | item...item}\n");
  fprintf(io,"\n");
  fprintf(io,"Creates an ASCII data file from an INFO data file.  A structure file,\n");
  fprintf(io,"listing the INFO item definitions, is also created.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Arguments:\n");
  fprintf(io,"\n");
  fprintf(io,"<info_file> -- The INFO data file to process.\n");
  fprintf(io,"\n");
  fprintf(io,"{ascii_file} -- The ASCII data file file to create.\n");
  fprintf(io,"\n");
  fprintf(io,"{structure_file} -- The structure file to create.\n");
  fprintf(io,"\n");
  fprintf(io,"{FIXED | LIST | 'character' | 'ascii_code'} -- Specifies the format\n");
  fprintf(io,"  of the ASCII data file.  The default is FIXED.\n");
  fprintf(io,"\n");
  fprintf(io,"{YYYYMMDD | MM/DD/YYYY | MM/DD/YY} -- Specifies the format to use for\n");
  fprintf(io,"  date items.  The default is YYYYMMDD for FIXED format files.  Other\n");
  fprintf(io,"  file formats default to MM/DD/YYYY.\n");
  fprintf(io,"\n");
  fprintf(io,"{NOQUOTE | QUOTE | SINGLEQUOTE} -- Specefies whether character fields\n");
  fprintf(io,"  should be unquoted, enclosed in double quotes, or enclosed in single\n");
  fprintf(io,"  quotes.  May be abbreviated to {N|Q|S}.  The default is NOQUOTE for\n");
  fprintf(io,"  FIXED format files.  Other file formats default to QUOTE (double\n");
  fprintf(io,"  quotes).\n");
  fprintf(io,"\n");
  fprintf(io,"{REDEFINED | item...item} -- Specifies which items to write to the\n");
  fprintf(io,"  ASCII file.  If you don't specify any items, all regular items will be\n");
  fprintf(io,"  included, but no redefined items.  This is the default.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Notes:\n");
  fprintf(io,"\n");
  fprintf(io,"Recognized formats for {ascii_file} are:\n");
  fprintf(io,"\n");
  fprintf(io,"  {FIXED} format files are in fixed width format, such that all item\n");
  fprintf(io,"  values are aligned on their starting columns, rather than being\n");
  fprintf(io,"  separated by a delimiter.  The QUOTE option is not available with this\n");
  fprintf(io,"  option.\n");
  fprintf(io," \n");
  fprintf(io,"  {LIST} format files have item values written to their full width and\n");
  fprintf(io,"  separated by a space.\n");
  fprintf(io,"\n");
  fprintf(io,"  {'character'} and {'ascii_code'} format files have item values striped\n");
  fprintf(io,"  of leading and trailing spaces and separated by a delimiter character\n");
  fprintf(io,"  you specify.  You can specify the delimiter either by typing the\n");
  fprintf(io,"  character itself, or by giving the character's ascii code.  For\n");
  fprintf(io,"  example, you could use ',' or '44' to get a comma delimited file.\n");
  fprintf(io,"  Some characters can cause problems when entered directly, such as TAB\n");
  fprintf(io,"  or '!'.  For these, you must use their ascii code equivalent.\n");
  fprintf(io,"\n");
  fprintf(io,"If you do not specify a name for {ascii_file}, it defaults to the name\n");
  fprintf(io,"of <info_file> with an extension added.  The default extension depends\n");
  fprintf(io,"on the format of the ASCII file:\n");
  fprintf(io,"\n");
  fprintf(io,"  {FIXED}         defaults to \".dat\"\n");
  fprintf(io,"  {LIST}          defaults to \".lis\"\n");
  fprintf(io,"  {'character'}   defaults to \".txt\"\n");
  fprintf(io,"  {'ascii_code'}  defaults to \".txt\"\n");
  fprintf(io,"\n");
  fprintf(io,"The default {structure_file} name is the same as <info_file> with a\n");
  fprintf(io,"\".def\" extension added.\n");
  fprintf(io,"\n");
  fprintf(io,"Both {ascii_file} and {structure_file} will be overwritten if they\n");
  fprintf(io,"already exist.\n");
  fprintf(io,"\n");
  fprintf(io,"If you want redefined items in addition to regular items, specify\n");
  fprintf(io,"REDEFINED (may be abbreviated to R).\n");
  fprintf(io,"\n");
  fprintf(io,"If you specify items to process, those (and only those) items will be\n");
  fprintf(io,"written out.  You can freely mix regular and redefined items.\n");
  fprintf(io,"However, you must enter the item names exactly as they appear in the\n");
  fprintf(io,"INFO file (upper or lower case).\n");
  fprintf(io,"\n");
  fprintf(io,"You can use \"#\" or \"@\" to skip optional arguments and accept the default.\n");
  fprintf(io,"\n");
  fprintf(io,"Available in ARC and stand-alone UNIX or DOS versions.\n");
  fprintf(io,"\n");
  fprintf(io,"The ARC version requires write access to the current workspace.\n");
  fprintf(io,"\n");
  fprintf(io,"See also ascii2info.\n");
  /* @ end help */
  return 1;
}
