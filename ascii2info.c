/* ascii2info.c
   ------------------------------------------------------------------------
   Creates an INFO data file from ascii data and structure files.
   
   Written: 05/24/93 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (All changes are by the author unless otherwise noted)
   
   08/26/94 Converted to write INFO file directly rather than as an e00
   file.  Now stand-alone C program; ARC/INFO not required.  Special
   arguments for when called by an AML so it can test for success.
   See ascii2info.aml.

   09/29/94 Allow double or single quotes as the quote character.
   
   12/28/94 Added in-line help.
   
   01/24/95 Elimiated fixed size arrays in favor of dynamically allocated
   structures.
   
   09/06/95 Eliminated all error checking on incoming date fields.
   This allows dates to be blank or empty without causing an error.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include INFOLIB_H
#include INFO_H

#define TRUE 1
#define FALSE 0

#define ASCMAXRECLEN INFO_MAXLEN * 2
#undef DEBUGGING

int help();
int writehelp();

int main(argc, argv)
     int    argc;
     char   *argv[];
{
  short int    numargs, status, done, i, ok, num, len, gousage;
  FILE         *ioin, *ioout;
  char         datafile[128], defsfile[128], errfile[128], infofile[128];
  char         temp[128];
  char         irec[ASCMAXRECLEN+2];
  short int    ffmt, defdfmt, defqopt, dfmt, quotechar, dlm;
  short int    itemfound, itemnum;
  char         vbuff[INFO_MAXLEN];
  TemplateList *asciiItems;
  long int     ascreclen;
  short int    ascnumitems;
  short int    *itemnums = ( short int * ) NULL;
  short int    doallitems, numitemstodo, numtodo;
  long int     recnum;
  char         progargs[320];
  char         dattag[5], deftag[5];
  short int    datafilearg=1, defsfilearg=2, infofilearg=3;
  short int    ffmtarg=4, dfmtarg=5, qoptarg=6;
  short int    itemarg=7;
  short int    verbose=FALSE, silent=FALSE;

#ifdef DEBUGGING
  char      numdecbuff[3];
  char      *itemcodes="_DCINBFLPVOT";
#endif

  /* These are for the INFO file. */
  
  char           *InfoFilePath;
  InfoFile       *FilePtr;
  TemplateList   *infoItems;
  long int        reclen, numrecs;
  short int       numitems, numredefitems;
  short int       itemstart, itemtype, itemilen, itemolen, itemnumdec;
  char            itemname[INFO_NAMELEN];
  

  /* Establish a look up list for input file formats. */
  
  char *ffmtlist="FIXED|LIST";
  
  /* Establish a look up list for date input formats. */
  
  char *dfmtlist="YYYYMMDD|MM/DD/YYYY|MM/DD/YY";
  
  /* Establish a look up list for quote argument. */
  
  char *quotelist="NOQUOTE|QUOTE|SINGLEQUOTE";
  
  
  /* Set program details. */
  
  char *unadargs="{-vs errorfile | -help }";
  char *progname="ASCII2INFO";
  char *progver= "5.7 July 13, 1998";
  int  authorid=TRUE;
  strcpy(progargs,"<data_file> {structure_file} {info_file}\n");
  strcat(progargs,
         "                  {FIXED | LIST | 'character' | 'ascii_code'}\n");
  strcat(progargs,"                  {YYYYMMDD | MM/DD/YYYY | MM/DD/YY}\n");
  strcat(progargs,"                  {NOQUOTE | QUOTE | SINGLEQUOTE}\n");
  strcat(progargs,"                  {item...item}");
  
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
                  datafilearg++;
                  defsfilearg++;
                  infofilearg++;
                  ffmtarg++;
                  dfmtarg++;
                  qoptarg++;
                  itemarg++;
                }
            }
          if (numargs < datafilearg)
            {
              usage(progname,progver,progargs,authorid);
              return FALSE;
            }
        }
    }
  
  
  /* Fetch arguments, check validity. */
  
  /* Fetch input file name. */
  
  strcpy(datafile,argv[datafilearg]);
  
  
  /* Fetch file input format. */

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
              dlm=i;
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
          dlm=temp[0];
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
          
          dlm='\0';        /* no delimiter */
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

  
  /* See if we can find input data file. */
  
  if (!sysexist(datafile))
    {
      strcpy(temp,datafile);
      strcat(temp,dattag);
      if (!sysexist(temp))
        {
          printf("%s: neither %s or %s exist.\n",progname,datafile,temp);
          return FALSE;
        }
      else
        strcpy(datafile,temp);
    }

  
  /* Fetch structure and output file names, if specified, or use defaults. */

  /* First get base filename, in case we need it to build default. */
  
  if (!strbefore(datafile,".",vbuff))
    strcpy(vbuff,datafile);


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
  if (!sysexist(defsfile))
    {
      strcpy(temp,defsfile);
      strcat(temp,deftag);
      if (!sysexist(temp))
        {
          printf("%s: neither %s or %s exist.\n",progname,defsfile,temp);
          return FALSE;
        }
      else
        strcpy(defsfile,temp);
    }
  
  
  /* Get (or build) INFO data file name. */
  
  if (numargs >= infofilearg)
    {
      strcpy(temp,argv[infofilearg]);
      if (strcmp(temp,"#") == 0 || strcmp(temp,"@") == 0)
        temp[0]='\0';
      else
        strcpy(infofile,temp);
    }
  else
    temp[0]='\0';
  if (temp[0] == '\0')
    strcpy(infofile,vbuff);
  strupr(infofile);
  InfoFilePath=infofile;

  
  /* Make sure structure and data files are different. */
  
  if (strcmp(defsfile,datafile) == 0)
    {
      printf("%s: Data and structure files must be different.\n",progname);
      return FALSE;
    }

  
  /* Fetch date input format. */
  
  /* If not specified or skipped, set to null, else use it. */
  
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
  
  /* If null, use default. */
  
  if (strnull(temp))
    strext(dfmtlist,defdfmt,temp,'|');
  
  /* Make sure it's one we know how to do. */
  
  dfmt=strkeyword(dfmtlist,temp,'|',"left");
  switch (dfmt)
    {
    case -1:        /* ambiguous */
    case  0:        /* not found */
      gousage=TRUE;
      break;
      
    case 1:         /* YYYYMMDD */
    case 2:         /* MM/DD/YYYY */
    case 3:         /* MM/DD/YY */
      break;
    }
  
  if (gousage)
    {
      usage(progname,progver,progargs,authorid);
      return FALSE;
    }
  
  
  /* Fetch quoted text option. */
  
  /* If not specified or skipped, set to null, else use it. */
  
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
  
  /* If null, use default. */
  
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
      printf("%s: Fixed width file can't have quoted strings.\n",progname);
      return FALSE;
    }
  
  if (gousage)
    {
      usage(progname,progver,progargs,authorid);
      return FALSE;
    }
  
  
  /* Check for specified item list. */
  
  doallitems=TRUE;
  numitemstodo=0;
  if (numargs >= itemarg)
    {
      
      /* Items were specified.  Count them. */
      
      doallitems=FALSE;
      for (i=itemarg; i <= numargs; i++)
        numitemstodo++;
    }
  
  
  /* Print arguments */
  
#ifdef DEBUGGING
  printf(" ASCII input file: %s\n",datafile);
  printf("   Structure file: %s\n",defsfile);
  printf(" Output INFO file: %s\n",InfoFilePath);
  
  printf("    Output format: ");
  if (ffmt < 100)
    {
      strext(ffmtlist,ffmt,temp,'|');
      printf("%s\n",temp);
    }
  else
    printf("\"%c\"\n",dlm);
  
  printf("     Input format: ");
  strext(ffmtlist,ffmt,temp,'|');
  printf("%s\n",temp);
  
  printf("      Date format: ");
  strext(dfmtlist,dfmt,temp,'|');
  printf("%s\n",temp);
  
  printf("            Quote: ");
  if (quotechar > 0)
    printf("%c\n",quotechar);
  else
    printf("none\n");
  
  printf("        Item list: ");
  if (doallitems)
    printf("All items will be processed.\n");
  else
    {
      for (i=itemarg; i <= numargs; i++)
        printf("%s ",argv[i]);
      putchar('\n');
    }
#endif
  
  
  /* Arguments check out.  Do the job. */
  
  if (verbose)
    printf("Creating INFO file %s from %s and %s...\n",
           InfoFilePath,datafile,defsfile);

  
  /* Open the structure file for reading. */
  
  if ((ioin = fopen(defsfile, "r")) == NULL)
    {
      printf("%s: Could not open structure file \"%s\".\n",progname,defsfile);
      return FALSE;
    }
  
  
  /* Read the item definitions from the structure file. */
  
  asciiItems=ascgetdefs(ioin,&ascnumitems,&ascreclen,dfmt,&status);
  if (status != INFO_OK)
    {
      infoerror(progname,status,ioin,ioout);
      return FALSE;
    }
  
  
  /* Close the structure file. */
  
  if (fclose(ioin) != 0)
    {
      printf("%s: Could not close input file \"%s\".\n",progname,defsfile);
      return FALSE;
    }

  
#ifdef DEBUGGING
  printf("%d items read from structure file.\n",ascnumitems);
  printf("Fixed width record length: %d\n",ascreclen);
  for (i=0; i < ascnumitems; i++)
    {
      itemstart=asciiItems[i].ItemPosition;
      strcpy(itemname,asciiItems[i].ItemName);
      itemtype=itemcodes[asciiItems[i].ItemType];
      itemilen=asciiItems[i].ItemWidth;
      itemolen=asciiItems[i].OutputWidth;
      itemnumdec=asciiItems[i].NumberDecimals;
      if (itemnumdec < 0)
        strcpy (numdecbuff,"  ");
      else
        sprintf(numdecbuff,"%2d",itemnumdec);
      printf("%4d %-16s %4d %4d %c %s\n",
             itemstart,itemname,itemilen,itemolen,itemtype,numdecbuff);
    }
#endif
  
  
  /* Set item numbers of items to be processed. */

  if (doallitems)
    numitemstodo=ascnumitems;
  
  if ((itemnums = (short int *)malloc(sizeof(short int)*numitemstodo)) == NULL)
    {
      infoerror(progname,INFO_MALLOC,ioin,ioout);
      return FALSE;
    }
  
  if (doallitems)
    for (num=0; num < ascnumitems; num++)
      itemnums[num]=num;
  else
    {
      itemfound=FALSE;
      for (i=0; i < numitemstodo; i++)
        {
          strcpy(temp,argv[i+itemarg]);
          strunquote(temp);
          itemfound=FALSE;
          for (num=0; num < ascnumitems; num++)
            if (strcmp(temp,asciiItems[num].ItemName) == 0)
              {
                itemnums[i]=num;
                itemfound=TRUE;
#ifdef DEBUGGING
                printf("In item %d is outitem %d\n",i+1,num+1);
#endif
                break;
              }
          if (!itemfound)
            {
              printf("%s: Item \"%s\" not found in structure file.\n",
                     progname,temp);
              return FALSE;
            }
        }
    }
  
  
  /* Open the input file for reading. */
  
  if ((ioin = fopen(datafile, "r")) == NULL)
    {
      printf("%s: Could not open input file \"%s\".\n",progname,datafile);
      return FALSE;
    }
  

  /* Delete existing output file. */

  /* Note: In future, change this so that, if the file already exists,
     we create a temporary file, and copy it over the original only if
     we successfully create the new file. */
  
  if (InfoFileExists(InfoFilePath))
    if (!InfoDeleteFile(InfoFilePath))
      {
        printf("%s: Could not delete existing INFO file \"%s\".\n",progname,
               InfoFilePath);
        fclose(ioin);
        return FALSE;
      }
  
  
  /* Create an empty output INFO file. */
  
  infoItems = (TemplateList *) NULL;
  status=InfoCreateFile(InfoFilePath,infoItems);
  if (status != INFO_OK)
    {
      printf("%s: Could not create output file \"%s\".\n",progname,
             InfoFilePath);
      fclose(ioin);
      return FALSE;
    }
  
  
  /* Define the output items in the INFO file. */
  
  for (i=0; i < numitemstodo; i++)
    {
      num=itemnums[i];
      strcpy(itemname,asciiItems[num].ItemName);
      itemtype=asciiItems[num].ItemType;
      itemilen=asciiItems[num].ItemWidth;
      itemolen=asciiItems[num].OutputWidth;
      itemnumdec=asciiItems[num].NumberDecimals;
      status=InfoDefineItem(InfoFilePath,itemname,itemilen,itemolen,
                            itemtype,itemnumdec);
      if (status != INFO_OK)
        {
          infoerror(progname,INFO_DEFINE_ITEM,ioin,ioout);
          return 0;
        }
    }      
  
  
  /* Get the item template for the INFO file we just created. */
  
  if((FilePtr = InfoOpenFile(InfoFilePath,InfoREAD)) == (InfoFile *) NULL)
    {
      infoerror(progname,INFO_OPEN_READ,ioin,ioout);
      return 0;
    }
  
  /* Normally we would feed numredefitems to getitems.  For this program,
     we don't want redef items, so we pass zero. */
  
  if ((infoItems = getitems(FilePtr,0)) == (TemplateList *) NULL)
    {
      infoerror(progname,INFO_READ_TEMPLATE,ioin,ioout);
      return 0;
    }
  

#ifdef DEBUGGING
  printf("Number of records=%d\n",InfoNumberRecords(FilePtr));
  printf("Record length=%d\n",InfoRecordLength(FilePtr));
  printf("Number of items=%d\n",InfoNumberItems(FilePtr));
  status=getnumredef(FilePtr,&i);
  printf("Number of redefined items=%d\n",i);
#endif
  

  /* Open the INFO file for writes. */
  
  if((FilePtr = InfoOpenFile(InfoFilePath,InfoWRITE)) == (InfoFile *) NULL)
    {
      InfoDeleteFile(InfoFilePath);
      infoerror(progname,INFO_OPEN_WRITE,ioin,ioout);
      return 0;
    }

  
  /* Read data records from the data file until end. */
  
  recnum = 0;
  while (fgets(irec, ASCMAXRECLEN, ioin) != NULL)
    {
      irec[strlen(irec)-1] = '\0';
      if (irec[0] != '\0')           /* Forgive blank lines. */
        {
          recnum++;

          
          /* Build an INFO data record of the desired items from the
             ascii data record. */
          
          status=recasc2info(irec,FilePtr,infoItems,numitemstodo,itemnums,
                             asciiItems,ffmt,dfmt,dlm,quotechar,recnum);
          if (status != INFO_OK)
            {
              infoerror(progname,status,ioout,ioin);
              InfoCloseFile(FilePtr);
              return 0;
            }
          
          
          /* Write the record to the INFO file. */
          
          if(!InfoWriteRecord(FilePtr,recnum))
            {
              infoderror(progname,INFO_WRITE_RECORD,FilePtr,ioin);
              return 0;
            }
        }

      
      /* Next record. */
      
    }
  
  
  /* Close files. */
  
  if (fclose(ioin) != 0)
    {
      printf("%s: Could not close input file \"%s\".\n",progname,datafile);
      return FALSE;
    }
  if(!InfoCloseFile(FilePtr))
    {
      printf("%s: Could not close output file \"%s\".\n",progname,InfoFilePath);
      return 0;
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
  fprintf(io,"Usage: ASCII2INFO <data_file> {structure_file} {info_file}\n");
  fprintf(io,"                  {FIXED | LIST | 'character' | 'ascii_code'}\n");
  fprintf(io,"                  {YYYYMMDD | MM/DD/YYYY | MM/DD/YY}\n");
  fprintf(io,"                  {NOQUOTE | QUOTE | SINGLEQUOTE}\n");
  fprintf(io,"                  {item...item}\n");
  fprintf(io,"\n");
  fprintf(io,"Creates an INFO data file from an ASCII data file and a structure file.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Arguments:\n");
  fprintf(io,"\n");
  fprintf(io,"<ascii_file> -- The ASCII data file to process.  Various formats are\n");
  fprintf(io,"  supported.  See notes below.\n");
  fprintf(io,"\n");
  fprintf(io,"{structure_file} -- The structure file.  Must contain INFO style item\n");
  fprintf(io,"  definitions for the ASCII data file.\n");
  fprintf(io,"\n");
  fprintf(io,"{info_file} -- The INFO data file to create.\n");
  fprintf(io,"\n");
  fprintf(io,"{FIXED | LIST | 'character' | 'ascii_code'} -- Specifies the format\n");
  fprintf(io,"  of the ASCII data file.  The default is FIXED.\n");
  fprintf(io,"\n");
  fprintf(io,"{YYYYMMDD | MM/DD/YYYY | MM/DD/YY} -- Specifies the format for date\n");
  fprintf(io,"  items in the ASCII data file.  The default is YYYMMDD for FIXED format\n");
  fprintf(io,"  files.  Other file formats default to MM/DD/YYYY.\n");
  fprintf(io,"\n");
  fprintf(io,"{NOQUOTE | QUOTE | SINGLEQUOTE} -- Specefies whether character fields\n");
  fprintf(io,"  are unquoted, enclosed in double quotes, or enclosed in single quotes.\n");
  fprintf(io,"  May be abbreviated to {N|Q|S}.  The default is NOQUOTE for FIXED\n");
  fprintf(io,"  format files.  Other file formats default to QUOTE (double quotes).\n");
  fprintf(io,"\n");
  fprintf(io,"{item...item} -- Specifies which items to write to the INFO file.\n");
  fprintf(io,"  If you don't specify any items, all items will be included.  This\n");
  fprintf(io,"  is the default.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Notes:\n");
  fprintf(io,"\n");
  fprintf(io,"Recognized formats for {ascii_file} are:\n");
  fprintf(io,"\n");
  fprintf(io,"  {FIXED} format files are in fixed width format, such that all item\n");
  fprintf(io,"  values are aligned on their starting columns, rather than being separated\n");
  fprintf(io,"  by a delimiter.\n");
  fprintf(io," \n");
  fprintf(io,"  {LIST} format files have item values separated by at least one space.\n");
  fprintf(io,"  Item values may not contain spaces unless they are quoted.\n");
  fprintf(io,"\n");
  fprintf(io,"  {'character'} and {'ascii_code'} format files have item values\n");
  fprintf(io,"  separated by a delimiter character you specify.  You can specify the\n");
  fprintf(io,"  delimiter either by typing the character itself, or by giving the\n");
  fprintf(io,"  character's ascii code.  For example, you could use ',' or '44' to read\n");
  fprintf(io,"  a comma delimited file.  Some characters can cause problems when\n");
  fprintf(io,"  entered directly, such as TAB or '!'.  For these, you can use their\n");
  fprintf(io,"  ascii code equivalent.\n");
  fprintf(io,"\n");
  fprintf(io,"The default {structure_file} name is the same as the <ascii_file> but\n");
  fprintf(io,"with a \".def\" extension.  This file must contain INFO item definitions\n");
  fprintf(io,"in the following format:\n");
  fprintf(io,"\n");
  fprintf(io,"  item_name item_width output_width item_type number_of_decimals\n");
  fprintf(io,"\n");
  fprintf(io,"  Examples:\n");
  fprintf(io,"  AREA                4   12 F  3\n");
  fprintf(io,"  SOILS-ID            4    5 B\n");
  fprintf(io,"  SOIL-CODE           3    3 C\n");
  fprintf(io,"  SYMBOL              3    3 I\n");
  fprintf(io,"\n");
  fprintf(io,"All the constraints imposed by INFO on item definitions apply.  The\n");
  fprintf(io,"values must be separated by at least one space or TAB.  INFO2ASCII\n");
  fprintf(io,"creates structure files in this format.  Item names will be defined in\n");
  fprintf(io,"the INFO file exactly as they appear in the structure file (upper or\n");
  fprintf(io,"lower case).\n");
  fprintf(io,"\n");
  fprintf(io,"The {info_file} name will be converted to uppercase.  The default\n");
  fprintf(io,"{info_file} name is the same as the <ascii_file> but with no\n");
  fprintf(io,"extension, and converted to uppercase.  If {info_file} already exists\n");
  fprintf(io,"it will be overwritten.\n");
  fprintf(io,"\n");
  fprintf(io,"If you specify items to process, those (and only those) items will be\n");
  fprintf(io,"written to the INFO file.\n");
  fprintf(io,"\n");
  fprintf(io,"You can use \"#\" or \"@\" to skip optional arguments and accept the default.\n");
  fprintf(io,"\n");
  fprintf(io,"Available in ARC and stand-alone UNIX or DOS versions.\n");
  fprintf(io,"\n");
  fprintf(io,"The ARC version requires write access to the current workspace.\n");
  fprintf(io,"\n");
  fprintf(io,"See also info2ascii.\n");
  /* @ end help */
  return 1;
}
