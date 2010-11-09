/* dbf2info.c
   ------------------------------------------------------------------------
   Creates an INFO data file from a dBASE dbf file.
   
   Written: 10/07/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (All changes are by the author unless otherwise noted)
      
   08/31/94 Converted to write INFO file directly rather than as an e00
   file.  Now stand-alone C program; ARC/INFO not required.  Special
   arguments for when called by an AML so it can test for success.
   See ascii2info.aml.  Added option to specify which fields to process.
   Got rid of fail|ignore|convert argument.  Now always ignore.

   09/07/94 Handle memo fields by converting to character.

   09/29/94 Changed some dbf file data types for compatibility on PC.
   
   12/16/94 Fixed file v directory bug on input dbf filename.
   
   12/28/94 Added in-line help.
   
   01/21/95 Elimiated fixed size arrays in favor of dynamically allocated
   structures.
   
   ------------------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H
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
  short int     numargs, done, i, index, status, pos;
  FILE          *ioout, *iodbf;
  char          ofile[128], dbffile[128], errfile[128];
  char          temp[128], dbfrec[DBF_MAXLEN+1], upopt, binopt;
  short int     doallfields, numfieldstodo, fieldfound, fieldnum, numtodo;
  short int     num, start;
  short int     *fieldnums = ( short int * ) NULL;
  short int     *fieldstarts = ( short int * ) NULL;
  long int      rec, jrec;
  short int     dbffilearg = 1, infofilearg = 2, binoptarg = 3, fieldarg = 4;
  short int     verbose=FALSE, silent=FALSE;
  char          progargs[160];
  
#ifdef DEBUGGING
  char          numdecbuff[3];
  char          *itemcodes="_DCINBFLPVOT";
#endif

  
  /* These are for the dbf file. */
  
  dbfField        *dbfFields;
  unsigned char   dbfid, month, day, year;
  long            lastrec;
  unsigned short  dataoffset, recsize, hasmemo;
  unsigned long   numfields, filesize;
  char            fieldname[DBF_NAMELEN], fieldtype, fieldnumdec;
  unsigned short  fieldlen;
  
  
  /* These are for the output INFO file. */
  
  char           *InfoFilePath;
  InfoFile       *FilePtr;
  TemplateList   *infoItems;
  long int        reclen, numrecs;
  short int       numitems, numredefitems;
  short int       itemtype, itemilen, itemolen, itemnumdec;
  char            itemname[INFO_NAMELEN];
  
  
  /* Set program details. */
  
  char *unadargs="{-vs errorfile | -help }";
  char *progname="DBF2INFO";
  char *progver= "5.7 July 13, 1998";
  int  authorid=TRUE;
  strcpy(progargs,"<dbf_file> {info_file} {BINARY | NOBINARY} {field...field}");
  
  /* Check number of arguments. */
  
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
      help(progname);
      return FALSE;
    }
  if (strcmp(temp,"-REV")==0)
    {
      strext(progver,1,temp,' ');
      printf("%s\n",temp);        
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
                  dbffilearg++;
                  infofilearg++;
                  binoptarg++;
                  fieldarg++;
                }
            }
          if (numargs < dbffilearg)
            {
              usage(progname,progver,progargs,authorid);
              return FALSE;
            }
        }
      
    }
  
  
  /* Fetch arguments, check validity. */
  
  /* Fetch input file name. */
  
  strcpy(dbffile,argv[dbffilearg]);

  
  /* See if we can find input dbf file. */

  status=FALSE;
  if (sysstat(dbffile,"exists"))
    {
      if (sysstat(dbffile,"isdir"))
        {
          strcat(dbffile,".dbf");
          if (sysstat(dbffile,"exists"))
            status=TRUE;
        }
      else
        status=TRUE;
    }
  else
    {
      strcat(dbffile,".dbf");
      if (sysstat(dbffile,"exists"))
        status=TRUE;
    }
  if (!status)
    {
      printf("%s: Input file \"%s\" not found.\n",progname,argv[dbffilearg]);
      return 0;
    }

  
  /* Get (or build) INFO data file name. */
  
  if (numargs >= infofilearg)
    {
      strcpy(temp,argv[infofilearg]);
      if (strcmp(temp,"#") == 0 || strcmp(temp,"@") == 0)
        temp[0]='\0';
      else
        strcpy(ofile,temp);
    }
  else
    temp[0]='\0';
  if (temp[0] == '\0')
    {
      if (!strbefore(dbffile,".",temp))
        strcpy(temp,dbffile);
      strcpy(ofile,temp);
    }
  strupr(ofile);
  InfoFilePath=ofile;
  
  
  /* Former argument: {FAIL | IGNORE | CONVERT}. Now always IGNORE. */
  
  upopt='I';
  
  
  if (numargs >= binoptarg)
    {
      strcpy(temp,argv[binoptarg]);
      if (strcmp(temp,"#") == 0 || strcmp(temp,"@") == 0)
        strcpy(temp,"BINARY");
      else
        {
          strupr(temp);
          if (temp[0]!='B' && temp[0]!='N')
            {
              usage(progname,progver,progargs,authorid);
              return FALSE;
            }
        }
    }
  else
    strcpy(temp,"BINARY");
  binopt=temp[0];

  
  /* Check for specified field list. */
  
  doallfields=TRUE;
  numfieldstodo=0;
  if (numargs >= fieldarg)
    {
      
      /* Fields were specified.  Count them. */
      
      doallfields=FALSE;
      for (i=fieldarg; i <= numargs; i++)
        numfieldstodo++;
    }
  
  
  /* Print arguments */
  
#ifdef DEBUGGING
  printf("DBF input file: %s\n",dbffile);
  printf("Output INFO file: %s\n",InfoFilePath);
  printf("Binary option: %c\n",binopt);
  printf("Field list: ");
  if (doallfields)
    printf("All fields will be processed.\n");
  else
    {
      for (i=fieldarg; i <= numargs; i++)
        printf("%s ",argv[i]);
      putchar('\n');
    }
#endif
  
  
  /* Arguments check out.  Do the job. */
  
  if (verbose)
    printf("Creating INFO file %s from %s...\n", InfoFilePath,dbffile);
  
  
  /* Open the input file for binary reading. */
  
  if ((iodbf = fopen(dbffile, "rb")) == NULL)
    {
      printf("%s: Could not open input file \"%s\".\n",progname,dbffile);
      return FALSE;
    }

  
  /* Read the header of the dbf file. */
  
  status=dbfgethead(iodbf,&dbfid,&month,&day,&year,&dataoffset,&recsize,
                    &lastrec,&filesize,&hasmemo,&numfields);
  if (status != DBF_OK)
    {
      dbferror(progname,status,iodbf,ioout);
      return FALSE;
    }
  
  
  /* Read the field definition section of the dbf file. */
  
  dbfFields=dbfgetdefs(iodbf,numfields,dataoffset,&status);
  if (status != DBF_OK)
    {
      dbferror(progname,status,iodbf,ioout);
      return FALSE;
    }
  
  
  /* Build a list of field start columns for each field in the dbf file. */
  
  if ((fieldstarts = (short int *)malloc(sizeof(short int)*numfields)) == NULL)
    {
      dbferror(progname,DBF_MALLOC,iodbf,ioout);
      return FALSE;
    }
  start=2; /* The delete flag! */
  for (num=0; num < numfields; num++)
    {
      fieldstarts[num]=start;
      start=start+dbfFields[num].fieldlen;
    }
  
  
  /* Set field numbers of fields to be processed. */
  
  if ((fieldnums = (short int *)malloc(sizeof(short int)*numfields)) == NULL)
    {
      dbferror(progname,DBF_MALLOC,iodbf,ioout);
      return FALSE;
    }
  
  if (doallfields)
    for (num=0; num < numfields; num++)
      {
        fieldnums[num]=num;
        numfieldstodo++;
      }
  else
    {
      fieldfound=FALSE;
      for (i=0; i < numfieldstodo; i++)
        {
          strcpy(temp,argv[i+fieldarg]);
          strunquote(temp);
          fieldfound=FALSE;
          for (num=0; num < numfields; num++)
            {
              if (strcmp(temp,dbfFields[num].fieldname) == 0)
                {
                  fieldnums[i]=num;
                  fieldfound=TRUE;
                  break;
                }
            }
          if (!(fieldfound))
            {
              printf("%s: Field \"%s\" not found in dbf file.\n",progname,temp);
              fclose(iodbf);
              return FALSE;
            }
        }
    }
  
  
  /* Convert the dbase field defs to appropriate INFO item defs. */
  
  infoItems=defsdbf2info(binopt,numfieldstodo,fieldnums,dbfFields,&status);
  if (status != DBF_OK)
    {
      dbferror(progname,status,iodbf,ioout);
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
        fclose(iodbf);
        return FALSE;
      }
  
  
  /* Create an empty output INFO file. */
  
  status=InfoCreateFile(InfoFilePath,NULL);
  if (status != INFO_OK)
    {
      printf("%s: Could not create output file \"%s\".\n",progname,
             InfoFilePath);
      fclose(iodbf);
      return FALSE;
    }
  
  
  /* Define the output items in the INFO file. */
  
  for (index=0; index < numfieldstodo; index++)
    {
      strcpy(itemname,infoItems[index].ItemName);
      itemtype=infoItems[index].ItemType;
      itemilen=infoItems[index].ItemWidth;
      itemolen=infoItems[index].OutputWidth;
      itemnumdec=infoItems[index].NumberDecimals;
      status=InfoDefineItem(InfoFilePath,itemname,itemilen,itemolen,
                            itemtype,itemnumdec);
      if (status != INFO_OK)
        {
          infoerror(progname,INFO_DEFINE_ITEM,iodbf,ioout);
          return 0;
        }
    }
  
  
  /* Open the INFO file we just created. */
  
  if((FilePtr = InfoOpenFile(InfoFilePath,InfoREAD)) == (InfoFile *) NULL)
    {
      infoerror(progname,INFO_OPEN_READ,iodbf,ioout);
      return 0;
    }

  
  /* Get the item template for the INFO file we just created. */
  
  free(infoItems);
  if ((infoItems = getitems(FilePtr,0)) == (TemplateList *) NULL)
    {
      infoerror(progname,INFO_READ_TEMPLATE,iodbf,ioout);
      return 0;
    }
  

  /* List the header terms to the screen. */
  
#ifdef DEBUGGING
  printf("DBF header terms...\n");
  printf("  Last update: %u/%u/%u\n",month,day,year);
  printf("  Data offset: %u\n",dataoffset);
  printf("  Record size: %u\n",recsize);
  printf("  Number of records: %ld\n",lastrec);
  printf("  Number of fields: %lu\n",numfields);
  printf("  File size: %lu\n",filesize);
  if (hasmemo)
    printf("  Memo fields present.\n");
  printf("INFO header terms...\n");
  printf("  Number of records=%ld\n",InfoNumberRecords(FilePtr));
  printf("  Record length=%d\n",InfoRecordLength(FilePtr));
  printf("  Number of items=%d\n",InfoNumberItems(FilePtr));
  status=getnumredef(FilePtr,&i);
  printf("  Number of redefined items=%d\n",i);
#endif
  
  
  /* List the item/field definitions to the screen. */
  
#ifdef DEBUGGING
  if (yesno("List item definitions? ",1))
    {
      printf("DBF field definitions...\n");
      for (index=0; index < numfieldstodo; index++)
        {
          fieldnum=fieldnums[index];
          strcpy(fieldname,dbfFields[fieldnum].fieldname);
          fieldtype=dbfFields[fieldnum].fieldtype;
          fieldlen=dbfFields[fieldnum].fieldlen;
          fieldnumdec=dbfFields[fieldnum].fieldnumdec;
          printf("%-11s %-4c %3u %3d\n",
                 fieldname,fieldtype,fieldlen,fieldnumdec);
        }
      printf("INFO item definitions...\n");
      for (index=0; index < numfieldstodo; index++)
        {
          strcpy(itemname,infoItems[index].ItemName);
          itemtype=itemcodes[infoItems[index].ItemType];
          itemilen=infoItems[index].ItemWidth;
          itemolen=infoItems[index].OutputWidth;
          itemnumdec=infoItems[index].NumberDecimals;
          if (itemnumdec < 0)
            strcpy (numdecbuff,"  ");
          else
            sprintf(numdecbuff,"%2d",itemnumdec);
          printf("%-16s %4d %4d %c %s\n",
                 itemname,itemilen,itemolen,itemtype,numdecbuff);
        }
    }
#endif
  
  
  /* Open the INFO file for writes. */
  
  if((FilePtr = InfoOpenFile(InfoFilePath,InfoWRITE)) == (InfoFile *) NULL)
    {
      InfoDeleteFile(InfoFilePath);
      infoerror(progname,INFO_OPEN_WRITE,iodbf,ioout);
      return 0;
    }
  
  
  /* Loop through the data section record by record. */
  
#ifdef DEBUGGING
  printf("Writing data section...\n");
#endif

  rec=0;
  for (jrec=1; jrec <= lastrec; jrec++)
    {
      
      /* Fetch a record from the dbf file. */
      
      status=dbfgetrec(iodbf,dbfrec,recsize,upopt);
      if (status != DBF_OK)
        {
          dbferror(progname,status,iodbf,ioout);
          return FALSE;
        }
      
      /* Process only if not a deleted record. */
      
      if (dbfrec[0] != '*')
        {
          rec++;
          
          /* Create an INFO data record of the specified dbf fields. */
          
          status=recdbf2info(dbfrec,FilePtr,infoItems,dbfFields,numfieldstodo,
                             fieldnums,fieldstarts,rec);
          if (status != DBF_OK)
            {
              InfoCloseFile(FilePtr);
              dbferror(progname,status,iodbf,ioout);
              return FALSE;
            }
          
          
          /* Write the record to the INFO file. */
          
          if(!InfoWriteRecord(FilePtr,rec))
            {
              infoderror(progname,INFO_WRITE_RECORD,FilePtr,iodbf);
              return FALSE;
            }
        }
      
      /* Next record. */
      
    }
  
  
  /* Close files. */
  
  if (fclose(iodbf) != 0)
    {
      printf("%s: Could not close input file \"%s\".\n",progname,dbffile);
      return FALSE;
    }
  if(!InfoCloseFile(FilePtr))
    {
      printf("%s: Could not close output file \"%s\".\n",progname,InfoFilePath);
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
  fprintf(io,"Usage: DBF2INFO <dbf_file> {info_file} {BINARY | NOBINARY} {field...field}\n");
  fprintf(io,"\n");
  fprintf(io,"Creates an INFO data file from a dBASE DBF file.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Arguments:\n");
  fprintf(io,"\n");
  fprintf(io,"<dbf_file> -- The dBASE DBF file to read.\n");
  fprintf(io,"\n");
  fprintf(io,"{info_file} -- The info data file to write.\n");
  fprintf(io,"\n");
  fprintf(io,"{BINARY | NOBINARY} -- Specifies how to translate type N or F fields that\n");
  fprintf(io,"  have no decimals.  May be abbreviated to {B|N}.  The default is\n");
  fprintf(io,"  BINARY.  See discussion below.\n");
  fprintf(io,"\n");
  fprintf(io,"{field...field} -- Specifies which fields to write to the INFO file.\n");
  fprintf(io,"  If you don't specify any fields, all fields will be included.  This\n");
  fprintf(io,"  is the default.\n");
  fprintf(io,"\n");
  fprintf(io,"Notes:\n");
  fprintf(io,"\n");
  fprintf(io,"<dbf_file> must be a standard dBASE DBF file.  Do not convert it to\n");
  fprintf(io,"unix format with dos2unix or d2u.\n");
  fprintf(io,"\n");
  fprintf(io,"The {info_file} name will be converted to uppercase.  The default\n");
  fprintf(io,"{info_file} name is the same as the <ascii_file> but with no\n");
  fprintf(io,"extension, and converted to uppercase.  If {info_file} already exists\n");
  fprintf(io,"it will be overwritten.\n");
  fprintf(io,"\n");
  fprintf(io,"Type C and D dBASE fields are the same in the INFO file.\n");
  fprintf(io,"\n");
  fprintf(io,"Type L dBASE fields are converted to type C INFO items.\n");
  fprintf(io,"\n");
  fprintf(io,"Type N and F dBASE fields with no decimals are converted as follows:\n");
  fprintf(io,"\n");
  fprintf(io,"          If you specify {BINARY}, and the field width is at\n");
  fprintf(io,"          least 5, but not more than 9, the field will be\n");
  fprintf(io,"          converted to a type B INFO item with an internal\n");
  fprintf(io,"          width of 4.\n");
  fprintf(io,"\n");
  fprintf(io,"          If the field width is less than 5, or greater than\n");
  fprintf(io,"          9, the field will be converted to a type I INFO\n");
  fprintf(io,"          item regardless of the {BINARY|NOBINARY} option.\n");
  fprintf(io,"\n");
  fprintf(io,"          If you specify {NOBINARY}, then the field will be\n");
  fprintf(io,"          are converted to type I INFO item regardless of\n");
  fprintf(io,"          the field width.\n");
  fprintf(io,"\n");
  fprintf(io,"Type N dBASE fields with decimals are converted as follows:\n");
  fprintf(io,"\n");
  fprintf(io,"          If the dBASE field width is 8 or less, the field\n");
  fprintf(io,"          will be converted to a type N INFO item.\n");
  fprintf(io,"\n");
  fprintf(io,"          If the dBASE field width is more than 8 but less\n");
  fprintf(io,"          than 10, the field will be converted to a type F\n");
  fprintf(io,"          INFO item with an internal width of 4.\n");
  fprintf(io,"\n");
  fprintf(io,"          If the dBASE field width is 10 or more, the field\n");
  fprintf(io,"          will be converted to a type F INFO item with an\n");
  fprintf(io,"          internal width of 8.\n");
  fprintf(io,"\n");
  fprintf(io,"Type F dBASE fields with decimals are converted as follows:\n");
  fprintf(io,"\n");
  fprintf(io,"          If the dBASE field width is less than 10, the\n");
  fprintf(io,"          field will be converted to a type F INFO item with\n");
  fprintf(io,"          an internal width of 4.\n");
  fprintf(io,"\n");
  fprintf(io,"          If the dBASE field width is 10 or more, the field\n");
  fprintf(io,"          will be converted to a type F INFO item with an\n");
  fprintf(io,"          internal width of 8.\n");
  fprintf(io,"\n");
  fprintf(io,"Type M dBASE fields are converted to 10 byte character fields.  The\n");
  fprintf(io,"memos themselves are not processed.\n");
  fprintf(io,"\n");
  fprintf(io,"The output width of the INFO items will always be the same as the\n");
  fprintf(io,"field width of the corresponding dBASE field.\n");
  fprintf(io,"\n");
  fprintf(io,"The item width (internal width) of the INFO items will be the same as\n");
  fprintf(io,"the output width, except type F INFO items, which always have an item\n");
  fprintf(io,"width of either 4 or 8, and type B INFO items, which always have an\n");
  fprintf(io,"item width of 4.\n");
  fprintf(io,"\n");
  fprintf(io,"If a type F value will not fit in its field as defined, conversion\n");
  fprintf(io,"will succeed, however INFO will use exponential notation to display\n");
  fprintf(io,"the value.  If the value still won't fit, INFO will display some\n");
  fprintf(io,"portion of the value and flag it with an asterisk.  This differs from\n");
  fprintf(io,"dBASE or FoxPro, where the decimal floats to accomodate larger or\n");
  fprintf(io,"smaller values.  To view the fields normally, you will need to use\n");
  fprintf(io,"ALTER to increase the output width.  Note that there is no loss of\n");
  fprintf(io,"data.  It is just a display diference.\n");
  fprintf(io,"\n");
  fprintf(io,"The {BINARY|NOBINARY} option has no affect other than how to convert\n");
  fprintf(io,"type N and F dbase fields with no decimals.  The default option uses\n");
  fprintf(io,"INFO type B items when it makes sense for more compact and efficient\n");
  fprintf(io,"INFO files.  If you prefer not to have any type B items in your INFO\n");
  fprintf(io,"file, use the NOBINARY option.\n");
  fprintf(io,"\n");
  fprintf(io,"If you specify fields to process, those (and only those) fields will\n");
  fprintf(io,"be written to the INFO file.  You must specify field names exactly as\n");
  fprintf(io,"they appear in the dbf file, ie, upper/lower case.\n");
  fprintf(io,"\n");
  fprintf(io,"You can use \"#\" or \"@\" to skip optional arguments and accept the default.\n");
  fprintf(io,"\n");
  fprintf(io,"Available in ARC and stand-alone UNIX or DOS versions.\n");
  fprintf(io,"\n");
  fprintf(io,"The ARC version requires write access to the current workspace.\n");
  fprintf(io,"\n");
  fprintf(io,"See also info2dbf.\n");
  /* @ end help */
  return 1;
}
