/* dbfread.c
   --------------------------------------------------------------------------
   Uses functions from my dbf library to read a dbase dbf file.
   Serves as a tester for the functions.
   Creates an ascii interchange file equivalent of the dbf file.
   This ascii file can then be converted to a new dbf file via dbfwrite.
   
   Written: 09/25/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (All changes by the author unless otherwise noted)
   
   01/21/95 Changed to use a structure for field defs rather than arrays.
   
   ------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H

#define TRUE 1
#define FALSE 0
#undef DEBUGGING

int main(argc, argv)
     int    argc;
     char   *argv[];
{
  short int      numargs, done, status;
  int            index;
  FILE           *ioin, *ioout;
  char           ifile[128], ofile[128];
  char           temp[128];
  char           irec[DBF_MAXLEN+1], orec[DBF_MAXLEN+1];
  
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
  
  char *progname="DBFREAD";
  char *progargs="<dbffile> <outfile>";
  char *progver= "2.0 January 21, 1995";
  int  authorid=FALSE;

  
  /* Check number of arguments. */
  
  numargs = argc - 1;
  if (numargs != 2)
    {
      usage(progname,progver,progargs,authorid);
      return 0;
    }
  
  
  /* Fetch arguments, check validity. */
  
  strcpy(ifile,argv[1]);
  strcpy(ofile,argv[2]);

  
  /* Append dot dbf? */
  
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

  
  /* Print arguments */
  
#ifdef DEBUGGING
  printf(" Input file: %s\n",ifile);
  printf("Output file: %s\n",ofile);
#endif
  
  
  /* Arguments check out.  Do the job. */
  
  /* Open the output file for writing. */
  
  if ((ioout = fopen(ofile, "w")) == NULL)
    {
      printf("%s: Could not open output file \"%s\".\n",progname,ofile);
      return 0;
    }
  
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
  
#ifdef DEBUGGING
  printf("Header:\n");
  printf("  Last update: %u/%u/%u\n",month,day,year);
  printf("  Data offset: %u\n",dataoffset);
  printf("  Record size: %u\n",recsize);
  printf("  Number of records: %ld\n",lastrec);
  printf("  Number of fields: %lu\n",numfields);
  printf("  File size: %lu\n",filesize);
  if (hasmemo)
    printf("  Memo fields present.\n");
#endif
  
  /* Write header to output file. */
  
  fprintf(ioout,"%u\n",dbfid);
  fprintf(ioout,"%u\n",year);
  fprintf(ioout,"%u\n",month);
  fprintf(ioout,"%u\n",day);
  fprintf(ioout,"%ld\n",lastrec);
  fprintf(ioout,"%u\n",dataoffset);
  fprintf(ioout,"%u\n",recsize);
  
  
  /* Read the field definition section of the dbf file. */
  
  dbfFields=dbfgetdefs(ioin,numfields,dataoffset,&status);
  if (status != DBF_OK)
    {
      dbferror(progname,status,ioin,ioout);
      return 0;
    }
  
  
  /* Write field definitions to the screen and output file. */
  
#ifdef DEBUGGING
  printf("Field definitions:\n");
#endif
  for (index=0; index < numfields; index++)
    {
      strcpy(fieldname,dbfFields[index].fieldname);
      fieldtype=dbfFields[index].fieldtype;
      fieldlen=dbfFields[index].fieldlen;
      fieldnumdec=dbfFields[index].fieldnumdec;
      fprintf(ioout,"%s,%c,%u,%d\n",
              fieldname,fieldtype,fieldlen,fieldnumdec);
#ifdef DEBUGGING
      printf("  %3d %-11s %-4c %3u %3d\n",
             index+1,fieldname,fieldtype,fieldlen,fieldnumdec);
#endif
    }
  
  
  /* Read the data section.  Write to screen and output file. */
  
#ifdef DEBUGGING
  printf("Data:\n");
#endif
  
  for (rec=1; rec <= lastrec; rec++)
    {
      status=dbfgetrec(ioin,irec,recsize,'C');
      if (status != DBF_OK)
        {
          dbferror(progname,status,ioin,ioout);
          return 0;
        }
      
      if (irec[0] != '*')
        fprintf(ioout,"%s\n",irec);
      
#ifdef DEBUGGING
      if (irec[0] != '*')
        printf("%s\n",irec);
      else
        printf("Record %d flagged as deleted.  Skipping...\n",rec);
#endif
      
    }
  
  
  /* Close files. */
  
  if (fclose(ioin) != 0)
    {
      printf("%s: Could not close input file \"%s\".\n",progname,ifile);
      return 0;
    }
  if (fclose(ioout) != 0)
    {
      printf("%s: Could not close output file \"%s\".\n",progname,ofile);
      return 0;
    }
  
  
  /* All done. */
  
  return 1;
}
