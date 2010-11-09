/* dbflook.c
   --------------------------------------------------------------------------
   Looks at a dbf file.
   
   Written: 09/30/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   10/19/92 Added the fields option.
   
   07/14/93 Took out skipping message for deleted records.  Added check
   for all records deleted.
   
   05/23/94 Seek input file as is first, then try default extension.
   
   07/18/94 Accept "f" or "n" for fields option.

   09/01/94 Changed fields option to {table|list}.

   09/29/94 Changed some dbf file data types for compatibility on PC.

   12/16/94 Fixed file v directory bug on input dbf filename.
   
   12/28/94 Added in-line help.
   
   01/21/95 Changed to use a structure for field defs rather than arrays.

   --------------------------------------------------------------------------
   */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H

#define TRUE 1
#define FALSE 0
#undef DEBUGGING

int help();
int writehelp();

int main(argc, argv)
     int    argc;
     char   *argv[];
{
  short int      numargs, done, status, dofields;
  int            index, pos, ilen;
  long int       numrecs, numdeleted;
  FILE           *ioin, *ioout;
  char           ifile[128], ofile[128];
  char           temp[128];
  char           irec[DBF_MAXLEN+1], orec[DBF_MAXLEN+1];
  char           vbuff[DBF_MAXLEN+1];
  unsigned char  byte;
  unsigned long  numbytes, rec;


  /* These are for the dbf file. */
  
  dbfField        *dbfFields;
  unsigned char   dbfid, month, day, year;
  long            lastrec;
  unsigned short  dataoffset, recsize, hasmemo;
  unsigned long   numfields, filesize;
  char            fieldname[DBF_NAMELEN], fieldtype, fieldnumdec;
  unsigned short  fieldlen;
  
  
  /* Set program details. */
  
  char *progname="DBFLOOK";
  char *progargs="<dbf_file> {TABLE | LIST}";
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
  
  strcpy(ifile,argv[1]);
  status=FALSE;
  if (sysstat(ifile,"exists"))
    {
      if (sysstat(ifile,"isdir"))
        {
          strcat(ifile,".dbf");
          if (sysstat(ifile,"exists"))
            status=TRUE;
        }
      else
        status=TRUE;
    }
  else
    {
      strcat(ifile,".dbf");
      if (sysstat(ifile,"exists"))
        status=TRUE;
    }
  if (!status)
    {
      printf("%s: Input file \"%s\" not found.\n",progname,argv[1]);
      return 0;
    }
  
  if (numargs >= 2)
    {
      strcpy(temp,argv[2]);
      if (strcmp(temp,"#") == 0 || strcmp(temp,"@") == 0)
        strcpy(temp,"TABLE");
      else
        {
          strupr(temp);
          if (temp[0]=='L')
            strcpy(temp,"LIST");
          if (temp[0]=='T')
            strcpy(temp,"TABLE");
          if (strcmp(temp,"LIST") && strcmp(temp,"TABLE"))
            {
              usage(progname,progver,progargs,authorid);
              return 0;
            }
        }
    }
  else
    strcpy(temp,"TABLE");
  dofields = !strcmp(temp,"LIST");
  
  
  /* Print arguments */
  
#ifdef DEBUGGING
  printf(" Input file: %s\n",ifile);
  if (dofields)
    printf("Fields will be listed one per line.\n");
  else
    printf("Records will be listed as a single line.\n");
#endif
  
  
  /* Arguments check out.  Do the job. */
  
  /* Open the input file for binary reading. */
  
  if ((ioin = fopen(ifile, "rb")) == NULL)
    {
      printf("%s: Could not open input file \"%s\".\n",progname,ifile);
      return 0;
    }
  
  
  /* Read the header of the dbf file. */
  
  status=dbfgethead(ioin,&dbfid,&month,&day,&year,&dataoffset,&recsize,
                    &lastrec,&filesize,&hasmemo,&numfields);
  if (status != DBF_OK)
    {
      dbferror(progname,status,ioin,ioout);
      return 0;
    }
  
  
  /* Print the header information to screen. */
  
  printf("  Last update: %u/%u/%u\n",month,day,year);
  printf("  Data offset: %u\n",dataoffset);
  printf("  Record size: %u\n",recsize);
  printf("  Number of records: %ld\n",lastrec);
  printf("  Number of fields: %lu\n",numfields);
  printf("  File size: %lu\n",filesize);
  if (hasmemo)
    printf("  Memo fields present.\n");
  
  
  /* List field definitions to the screen. */
  
  if (yesno("List field definitions? ",1))
    {
      
      /* Read the field definition section of the dbf file. */
      
      dbfFields=dbfgetdefs(ioin,numfields,dataoffset,&status);
      if (status != DBF_OK)
        {
          dbferror(progname,status,ioin,ioout);
          return 0;
        }
      
      /* List the field definition section of the dbf file. */
      
      for (index=0; index < numfields; index++)
        {
          strcpy(fieldname,dbfFields[index].fieldname);
          fieldtype=dbfFields[index].fieldtype;
          fieldlen=dbfFields[index].fieldlen;
          fieldnumdec=dbfFields[index].fieldnumdec;
          printf("  %3d %-11s %-4c %3u %3d\n",
                 index+1,fieldname,fieldtype,fieldlen,fieldnumdec);
        }
      
      
      /* List the data section to the screen. */
      
      if (yesno("List data records? ",1))
        {
          numrecs=0;
          numdeleted=0;
          for (rec=1; rec <= lastrec; rec++)
            {
              status=dbfgetrec(ioin,irec,recsize,'C');
              if (status != DBF_OK)
                {
                  dbferror(progname,status,ioin,ioout);
                  return 0;
                }
              
#ifdef DEBUGGING
              printf("REC=%s\n",irec);
#endif
              
              if (irec[0] == '*')
                {
                  numdeleted++;
                }
              else
                {
                  numrecs++;
                  if (dofields)
                    {
                      printf("     Record %ld\n",rec);                  
                      pos=1;
                      for (index=0; index < numfields; index++)
                        {
                          strcpy(fieldname,dbfFields[index].fieldname);
                          ilen=dbfFields[index].fieldlen;
                          strsubstr(irec,vbuff,pos,ilen);
                          printf("%-11s=%s\n",fieldname,vbuff);
                          pos=pos+ilen;
                        }
                    }
                  else
                    {
                      pos=1;
                      for (index=0; index < numfields; index++)
                        {
                          ilen=dbfFields[index].fieldlen;
                          strsubstr(irec,vbuff,pos,ilen);
                          printf("%s ",vbuff);
                          pos=pos+ilen;
                        }
                      putchar('\n');
                    }
                }
            }
          if(numrecs == 0)
            {
              if (numdeleted > 0)
                printf("All records are flagged as deleted!\n");
              else
                printf("No data records!\n");
            }
        }
    }
  
  
  /* Close files. */
  
  if (fclose(ioin) != 0)
    {
      printf("%s: Could not close input file \"%s\".\n",progname,ifile);
      return 0;
    }
  
  
  /* All done. */
  
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
  fprintf(io,"Usage: dbflook <dbf_file> {TABLE | LIST}\n");
  fprintf(io,"\n");
  fprintf(io,"Allows you to examine the contents of a dBASE DBF file.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Arguments:\n");
  fprintf(io,"\n");
  fprintf(io,"<dbf_file> -- The dbf file to examine.\n");
  fprintf(io,"\n");
  fprintf(io,"{TABLE | LIST} -- Specifies whether fields will be listed\n");
  fprintf(io,"  individualy on separate lines, or all together on a single line.\n");
  fprintf(io,"  May be abbreviated to {T|L}.  The default is TABLE.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Notes:\n");
  fprintf(io,"\n");
  fprintf(io,"<dbf_file> must be a standard dBASE DBF file.  Do not convert it unix\n");
  fprintf(io,"format with dos2unix or d2u.\n");
  fprintf(io,"\n");
  fprintf(io,"The header terms will be listed.\n");
  fprintf(io,"\n");
  fprintf(io,"You will then be presented with an opportunity to list the field definitions.\n");
  fprintf(io,"If you decline, the program ends.\n");
  fprintf(io,"\n");
  fprintf(io,"If you listed the field definitions, you will then be presented with an\n");
  fprintf(io,"opportunity to list data records.  If you decline, the program ends.\n");
  fprintf(io,"\n");
  fprintf(io,"See also dbflist and dbfitems.\n");
  /* @ end help */
  return 1;
}
