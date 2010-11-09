/* info2dbf.c
   ------------------------------------------------------------------------
   Creates a dbf file from an INFO data file.
   
   Written: 10/05/92 Randy Deardorff.  US EPA Seattle.
   rdeardor@r0serv.r10.epa.gov
   
   Modification log: (All changes are by the author unless otherwise noted)
   
   08/30/94 Converted to read INFO file directly rather than from e00
   file.  Now stand-alone C program; ARC/INFO not required.  Special
   arguments for when called by an AML so it can test for success.
   See info2dbf.aml.  Added option to specify which items to process.
   Added logic to resolve duplicate field names.
   
   09/29/94 Changed some dbf file data types for compatibility on PC.
   
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
  FILE          *ioin, *iodbf;
  char          ifile[128], dbffile[128], errfile[128];
  char          temp[128], orec[INFO_MAXLEN+1];
  short int     dbver;
  short int     doallitems, numitemstodo, itemfound, itemnum, numtodo, num;
  short int     *itemnums = ( short int * ) NULL;
  char          vbuff[INFO_MAXLEN];
  short int     infofilearg = 1, dbffilearg = 2, dbverarg = 3, itemarg = 4;
  long int      rec;
  short int     verbose=FALSE, silent=FALSE;
  
#ifdef DEBUGGING
  char          numdecbuff[3];
  char          *itemcodes="_DCINBFLPVOT";
#endif

  
  /* These are for the dbf file. */
  
  dbfTemplate    *dbfFields;
  unsigned char   dbfid, month, day, year;
  long            lastrec;
  unsigned short  dataoffset, recsize, hasmemo;
  unsigned long   numfields, filesize;
  char            fieldname[DBF_NAMELEN], fieldtype, fieldnumdec;
  unsigned short  fieldlen;

  
  /* These are for the INFO file. */
  
  char           *InfoFilePath;
  InfoFile       *FilePtr;
  TemplateList   *infoItems;
  long int        reclen, numrecs;
  short int       numitems, numredefitems, doredefined;
  short int       startcol, itemtype, itemilen, itemolen, itemnumdec;
  char            itemname[INFO_NAMELEN];
  
  /* Set program details. */
  
  char *unadargs="{-vs errorfile | -help }";
  char *progname="INFO2DBF";
  char *progver= "5.7 July 13, 1998";
  char *progargs="<info_file> {dbf_file} {IV|III} {REDEFINED | item...item}";
  int  authorid=TRUE;
  
  
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
                  dbffilearg++;
                  dbverarg++;
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
  
  /* Fetch output file name, if specified, or use default. */

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
  strlwr(vbuff);

  
  /* Get (or build) data file name. */
  
  if (numargs >= dbffilearg)
    {
      strcpy(temp,argv[dbffilearg]);
      if (strcmp(temp,"#") == 0 || strcmp(temp,"@") == 0)
        temp[0]='\0';
      else
        strcpy(dbffile,temp);
    }
  else
    temp[0]='\0';
  if (temp[0] == '\0')
    {
      strcpy(dbffile,vbuff);
      strcat(dbffile,".dbf");
    }
  else
    if (strfind(dbffile,".dbf",0) < 0)
      strcat(dbffile,".dbf");

  
  /* Get dBASE version argument. */
  
  if (numargs >= dbverarg)
    {
      strcpy(temp,argv[dbverarg]);
      if (strcmp(temp,"#") == 0 || strcmp(temp,"@") == 0)
        strcpy(temp,"IV");
      else
        {
          strupr(temp);
          if (strcmp(temp,"IV") && strcmp(temp,"III"))
            {
              usage(progname,progver,progargs,authorid);
              return FALSE;
            }
        }
    }
  else
    strcpy(temp,"IV");
  if (strcmp(temp,"III")==0)
    dbver=3;
  if (strcmp(temp,"IV")==0)
    dbver=4;


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
        
        /* Items were specified.  Count them them. */
        
        doallitems=FALSE;
        doredefined=TRUE;
        for (i=itemarg; i <= numargs; i++)
          numitemstodo++;
      }
  
  
  /* Print arguments */
  
#ifdef DEBUGGING
  printf(" Input file: %s\n",ifile);
  printf("Output file: %s\n",dbffile);
  printf("Output type: %d\n",dbver);

  printf("  Item list: ");
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
    printf("Creating %s from INFO file %s...\n",dbffile,InfoFilePath);
  
  
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
      infoderror(progname,status,FilePtr,iodbf);
      return FALSE;
    }

  
  /* Fetch item defs. */
  
  if ((infoItems = getitems(FilePtr, numredefitems)) == (TemplateList *) NULL)
    {
      infoderror(progname,INFO_READ_TEMPLATE,FilePtr,iodbf);
      return FALSE;
    }
  
  
  /* Set item numbers of items to be processed. */

  if (doredefined)
    numtodo=numitems+numredefitems;
  else
    numtodo=numitems;
  
  if ((itemnums = (short int *)malloc(sizeof(short int)*numtodo)) == NULL)
    {
      infoderror(progname,INFO_MALLOC,FilePtr,iodbf);
      return FALSE;
    }
  
  if (doallitems)
    for (num=0; num < numtodo; num++)
      {
        itemnums[num]=num;
        numitemstodo++;
      }
  else
    {
      itemfound=FALSE;
      for (i=0; i < numitemstodo; i++)
        {
          strcpy(temp,argv[i+itemarg]);
          strunquote(temp);
          itemfound=FALSE;
          for (num=0; num < numtodo; num++)
            if (strcmp(temp,infoItems[num].ItemName) == 0)
              {
                itemnums[i]=num;
                itemfound=TRUE;
                break;
              }
          if (!(itemfound))
            {
              printf("%s: Item \"%s\" not found in INFO file.\n",progname,temp);
              InfoCloseFile(FilePtr);
              return FALSE;
            }
        }
    }
  
  
  /* Open the output dbf file for binary writing. */
  
  if ((iodbf = fopen(dbffile, "wb")) == NULL)
    {
      printf("%s: Could not open output file \"%s\".\n",progname,dbffile);
      return FALSE;
    }
  
  
  /* Create appropriate dBASE field defs. */
  
  dbfFields=defsinfo2dbf(infoItems,numitems,numredefitems,numitemstodo,itemnums,
                         doredefined,dbver,&numfields,&recsize,&status);
  if (status != INFO_OK)
    {
      infoerror(progname,status,ioin,iodbf);
      return FALSE;
    }
  
  
  /* Create appropriate dbf header terms. */
  
  status=headinfo2dbf(numrecs,numfields,recsize,&dbfid,&month,&day,&year,
                     &dataoffset,&lastrec,&filesize,&hasmemo);
  if (status != INFO_OK)
    {
      infoerror(progname,status,ioin,iodbf);
      return FALSE;
    }
  
  
  /* Set date updated to today. */
  
  status=dbfgetdate(&month,&day,&year);
  if (status != DBF_OK)
    {
      dbferror(progname,status,ioin,iodbf);
      return FALSE;
    }

  
  /* List the header terms to the screen. */
  
#ifdef DEBUGGING
  printf("INFO header terms...\n");
  printf("  Record size: %ld\n",reclen);
  printf("  Number of records: %ld\n",numrecs);
  printf("  Number of items: %d\n",numitems);
  printf("  Number of redefined items: %d\n",numredefitems);
  printf("DBF header terms...\n");
  printf("  Last update: %u/%u/%u\n",month,day,year);
  printf("  Data offset: %u\n",dataoffset);
  printf("  Record size: %u\n",recsize);
  printf("  Number of records: %ld\n",lastrec);
  printf("  Number of fields: %ld\n",numfields);
  printf("  File size: %ld\n",filesize);
  if (hasmemo)
    printf("  Memo fields present.\n");
#endif
  
  
  /* List the item and field definitions to the screen. */
  
#ifdef DEBUGGING
  if (yesno("List item definitions? ",1))
    {
      printf("INFO item definitions...\n");
      for (index=0; index < numitemstodo; index++)
        {
          itemnum=itemnums[index];
          strcpy(itemname,infoItems[itemnum].ItemName);
          itemtype=itemcodes[infoItems[itemnum].ItemType];
          itemilen=infoItems[itemnum].ItemWidth;
          itemolen=infoItems[itemnum].OutputWidth;
          itemnumdec=infoItems[itemnum].NumberDecimals;
          if (itemnumdec < 0)
            strcpy (numdecbuff,"  ");
          else
            sprintf(numdecbuff,"%2d",itemnumdec);
          printf("%-16s %4d %4d %c %s\n",
                  itemname,itemilen,itemolen,itemtype,numdecbuff);
        }
      printf("DBF field definitions...\n");
      for (index=0; index < numfields; index++)
        {
          strcpy(fieldname,dbfFields[index].fieldname);
          fieldtype=dbfFields[index].fieldtype;
          fieldlen=dbfFields[index].fieldlen;
          fieldnumdec=dbfFields[index].fieldnumdec;
          printf("%-11s %-4c %3u %3d\n",
                 fieldname,fieldtype,fieldlen,fieldnumdec);
        }
    }
#endif
  
  
  /* Write the header of the dbf file. */
  
#ifdef DEBUGGING
  printf("Writing header...\n");
#endif
  status=dbfputhead(iodbf,dbfid,month,day,year,dataoffset,recsize,lastrec);
  if (status != DBF_OK)
    {
      dbferror(progname,status,ioin,iodbf);
      return FALSE;
    }
  
  
  /* Write the field definition section of the dbf file. */
  
#ifdef DEBUGGING
  printf("Writing field definitions...\n");
#endif
  
  status=dbfputdefs(iodbf,numfields,dbfFields);
  if (status != DBF_OK)
    {
      dbferror(progname,status,ioin,iodbf);
      return FALSE;
    }
  
  
  /* Loop through the data section record by record. */
  
#ifdef DEBUGGING
  printf("Writing data section...\n");
#endif
  
  for (rec=1; rec <= FilePtr->NumberRecords; rec++)
    {
      
      /* Read a record from the INFO file. */
      
      if(!InfoReadRecord(FilePtr,rec))
        {
          infoderror(progname,INFO_READ_RECORD,FilePtr,iodbf);
          return FALSE;
        }
      
      /* Build an apropriate dbf record from the INFO record. */
      
      status=recinfo2dbf(FilePtr,infoItems,orec,itemnums,numitemstodo,rec);
      if (status != INFO_OK)
        {
          infoerror(progname,status,ioin,iodbf);
          return FALSE;
        }
      
      /* Write the dbf record to the dbf file. */
      
      status=dbfputrec(iodbf,orec,recsize);
      if (status != DBF_OK)
        {
          dbferror(progname,status,ioin,iodbf);
          return FALSE;
        }
    }
  
  
  /* Write the tail section of the dbf file. */
  
#ifdef DEBUGGING
  printf("Writing tail...\n");
#endif
  status=dbfputtail(iodbf);
  if (status != DBF_OK)
    {
      dbferror(progname,status,ioin,iodbf);
      return FALSE;
    }
  
  
  /* Close files. */
  
  if(!InfoCloseFile(FilePtr))
    {
      printf("%s: Could not close input file \"%s\".\n", progname,InfoFilePath);
      return FALSE;
    }
  if (fclose(iodbf) != 0)
    {
      printf("%s: Could not close output file \"%s\".\n",progname,dbffile);
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
  fprintf(io,"Usage: INFO2DBF <info_file> {dbf_file} {IV|III} {REDEFINED | item...item}\n");
  fprintf(io,"\n");
  fprintf(io,"Creates an dBASE DBF file from an INFO data file.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Arguments:\n");
  fprintf(io,"\n");
  fprintf(io,"<info_file> -- The INFO data file to read.\n");
  fprintf(io,"\n");
  fprintf(io,"{dbf_file} -- The dBASE DBF file to write.\n");
  fprintf(io,"\n");
  fprintf(io,"{IV | III} -- Specifies whether the ouput file will be a dBASE IV or a\n");
  fprintf(io,"  dBASE III+ type dbf file.  The default is IV.\n");
  fprintf(io,"\n");
  fprintf(io,"{REDEFINED | item...item} -- Specifies which items to write to the\n");
  fprintf(io,"  dBASE file.  If you don't specify any items, all regular items will be\n");
  fprintf(io,"  included, but no redefined items.  This is the default.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Notes:\n");
  fprintf(io,"\n");
  fprintf(io,"If you do not specify a name for {dbf_file}, it defaults to the name\n");
  fprintf(io,"of <info_file> with the extension \".dbf\" added.  The extension \".dbf\"\n");
  fprintf(io,"will be added to {dbf_file} if not supplied.\n");
  fprintf(io,"\n");
  fprintf(io,"If {dbf_file} already exists it will be overwritten.\n");
  fprintf(io,"\n");
  fprintf(io,"The output type {IV|III} has two implications.  The first is item\n");
  fprintf(io,"types.  dBASE III+ does not support type F.  When you specify type III\n");
  fprintf(io,"output, all type F items in the INFO file will be converted to type N\n");
  fprintf(io,"in the dBASE file.  For type IV output, type F info items are written\n");
  fprintf(io,"to the dbf file as type F.  The second implication is number if items.\n");
  fprintf(io,"Type III allows a maximum of 128 items.  Type IV allows 255.\n");
  fprintf(io,"\n");
  fprintf(io,"Item names will be truncated to ten characters.  All illegal\n");
  fprintf(io,"characters in item names will be converted to underscores.  The\n");
  fprintf(io,"program checks for duplicate field names and eliminates them by\n");
  fprintf(io,"appending the field number to the field name.\n");
  fprintf(io,"\n");
  fprintf(io,"All fields in the dBASE file will have their field lengths set to the\n");
  fprintf(io,"output widths of the corresponding INFO items.  The only exception are\n");
  fprintf(io,"date fields, where the field length is always 8.\n");
  fprintf(io,"\n");
  fprintf(io,"Type N and F items must be defined such that the number of decimals is\n");
  fprintf(io,"no more than the output width less 2.  INFO allows the number of\n");
  fprintf(io,"decimals to be the output width less 1, but dBASE insists on 2 less.\n");
  fprintf(io,"The program checks for this and fails with a message if found.\n");
  fprintf(io,"\n");
  fprintf(io,"Type I and B INFO items are converted to type N dBASE fields with no\n");
  fprintf(io,"decimals.\n");
  fprintf(io,"\n");
  fprintf(io,"Type C INFO items are the same in the dBASE file.\n");
  fprintf(io,"\n");
  fprintf(io,"Type F and N INFO items are the same in the dBASE file, unless you\n");
  fprintf(io,"have specified type III output, in which case type F INFO items are\n");
  fprintf(io,"converted to type N dBASE fields.\n");
  fprintf(io,"\n");
  fprintf(io,"Only item types I, B, C, D, F, and N are allowed.  If any other item\n");
  fprintf(io,"types are found the program will halt with an error message.\n");
  fprintf(io,"\n");
  fprintf(io,"Date of last update will be set to the current date.\n");
  fprintf(io,"\n");
  fprintf(io,"If you want redefined items in addition to regular items, specify\n");
  fprintf(io,"REDEFINED (may be abbreviated to R).\n");
  fprintf(io,"\n");
  fprintf(io,"If you specify items to process, those (and only those) items will be\n");
  fprintf(io,"written to the dBASE file.  You can freely mix regular and redefined\n");
  fprintf(io,"items.  However, you must enter the item names exactly as they appear\n");
  fprintf(io,"in the INFO file, ie, upper or lower case.\n");
  fprintf(io,"\n");
  fprintf(io,"You can use \"#\" or \"@\" to skip optional arguments and accept the default.\n");
  fprintf(io,"\n");
  fprintf(io,"Available in ARC and stand-alone UNIX or DOS versions.\n");
  fprintf(io,"\n");
  fprintf(io,"The ARC version requires write access to the current workspace.\n");
  fprintf(io,"\n");
  fprintf(io,"See also dbf2info.\n");
  /* @ end help */
  return 1;
}
