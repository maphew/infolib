/* dbfwrite.c
   --------------------------------------------------------------------------
   Uses functions from my dbf library to write a dbase dbf file.
   Serves as a tester for the functions.
   Reads an ascii file created by dbfread.c to create the dbf file.
   
   Note that when we create a dbf file, all unused bytes in the header
   and field definition sections are set to nulls.  Dbf files created by
   dBASE do not necessarily follow this plan.  Thus, if you dump a dbf
   file that dBASE created to an ascii file (via dbfread) then create a
   new dbf file from that ascii file (via dbfwrite) the two dbf files
   will have differences as shown by diff, but since all the differences
   are restricted to the unused bytes, they are the same as far as dBASE
   is concerned.
   
   Written: 09/30/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
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
  
  char *progname="DBFWRITE";
  char *progargs="<infile> <dbffile>";
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
  
  if (strfind(ofile,".dbf",0) < 0)
    strcat(ofile,".dbf");
  
  
  /* Print arguments */
  
#ifdef DEBUGGING
  printf(" Input file: %s\n",ifile);
  printf("Output file: %s\n",ofile);
#endif
  
  
  /* Arguments check out.  Do the job. */
  
  /* Open the input file for reading. */
  
  if ((ioin = fopen(ifile, "r")) == NULL)
    {
      printf("%s: Could not open input file \"%s\".\n",progname,ifile);
      return 0;
    }
  
  /* Open the output file for binary writing. */
  
  if ((ioout = fopen(ofile, "wb")) == NULL)
    {
      printf("%s: Could not open output file \"%s\".\n",progname,ofile);
      return 0;
    }
  
  
  /* Read header terms from the input file. */
  
  fgets(temp,DBF_MAXLEN,ioin);
  dbfid=atoi(temp);
  fgets(temp,DBF_MAXLEN,ioin);
  year=atoi(temp);
  fgets(temp,DBF_MAXLEN,ioin);
  month=atoi(temp);
  fgets(temp,DBF_MAXLEN,ioin);
  day=atoi(temp);
  fgets(temp,DBF_MAXLEN,ioin);
  lastrec=atoi(temp);
  fgets(temp,DBF_MAXLEN,ioin);
  dataoffset=atoi(temp);
  fgets(temp,DBF_MAXLEN,ioin);
  recsize=atoi(temp);
  
  /* Calculate other terms implied by header terms. */
  
  numfields=(dataoffset-DBF_BUFFSIZE-1)/DBF_BUFFSIZE;
  filesize=(recsize*lastrec)+dataoffset+1;
  switch (dbfid)
    {
    case 3:
      hasmemo=FALSE;
      break;
    case 131:
      hasmemo=TRUE;
      break;
    default:
      dbferror(progname,DBF_BAD_DBFID,ioin,ioout);
      return 0;
    }
  
  /* Set date updated to today. */
  
  status=dbfgetdate(&month,&day,&year);
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
  printf("  Number of fields: %ld\n",numfields);
  printf("  File size: %ld\n",filesize);
  if (hasmemo)
    printf("  Memo fields present.\n");
#endif
  
  
  /* Write the header of the dbf file. */
  
#ifdef DEBUGGING
  printf("Writing header...\n");
#endif
  
  status=dbfputhead(ioout,dbfid,month,day,year,dataoffset,recsize,lastrec);
  if (status != DBF_OK)
    {
      dbferror(progname,status,ioin,ioout);
      return 0;
    }

  
  /* Allocate memory for the field def structure. */
  
  if ((dbfFields = (dbfField *) malloc(sizeof(dbfField)*numfields)) == NULL)
    {
      dbferror(progname,DBF_MALLOC_FIELD,ioin,ioout);
      return 0;
    }
  
  
  /* Read field definitions from the input file. */
  
  for (index=0; index < numfields; index++)
    {
      fgets(irec,DBF_MAXLEN,ioin);
      strext(irec,1,fieldname,',');
      
      strext(irec,2,temp,',');
      fieldtype=temp[0];
      
      strext(irec,3,temp,',');
      fieldlen=atoi(temp);
      
      strext(irec,4,temp,',');
      fieldnumdec=atoi(temp);

      /* Place the values in the structure. */

      strcpy(dbfFields[index].fieldname, fieldname);
      dbfFields[index].fieldtype = fieldtype;
      dbfFields[index].fieldlen = fieldlen;
      dbfFields[index].fieldnumdec = fieldnumdec;

    }
  
  /* List field definitions from the arrays to the screen. */
  
#ifdef DEBUGGING
  printf("Field definitions:\n");
  for (index=0; index < numfields; index++)
    {
      strcpy(fieldname, dbfFields[index].fieldname);
      fieldtype = dbfFields[index].fieldtype;
      fieldlen = dbfFields[index].fieldlen;
      fieldnumdec = dbfFields[index].fieldnumdec;
      printf("  %3d %-11s %-4c %3u %3d\n",
             index+1,fieldname,fieldtype,fieldlen,fieldnumdec);
    }
#endif
  
  
  /* Write the field definition section of the dbf file. */
  
#ifdef DEBUGGING
  printf("Writing field definitions...\n");
#endif
  status=dbfputdefs(ioout,numfields,dbfFields);
  if (status != DBF_OK)
    {
      dbferror(progname,status,ioin,ioout);
      return 0;
    }
  
  
  /* Read the data section from the input file and write to the dbf file. */
  
#ifdef DEBUGGING
  printf("Writing data section...\n");
#endif
  
  for (rec=1; rec <= lastrec; rec++)
    {
      fgets(irec,DBF_MAXLEN,ioin);
      
#ifdef DEBUGGING
      printf("%s",irec);
#endif
      status=dbfputrec(ioout,irec,recsize);
      if (status != DBF_OK)
        {
          dbferror(progname,status,ioin,ioout);
          return 0;
        }
    }
  
  
  /* Write the tail section of the dbf file. */
  
#ifdef DEBUGGING
  printf("Writing tail...\n");
#endif
  
  status=dbfputtail(ioout);
  if (status != DBF_OK)
    {
      dbferror(progname,status,ioin,ioout);
      return 0;
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
