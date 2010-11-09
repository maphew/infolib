/* dbfitems.c
   --------------------------------------------------------------------------
   Lists the field definitions of a dbf file.
   
   Written: 07/28/93 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   05/23/94 Seek input file as is first, then try default extension.

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
  
  char *progname="DBFITEMS";
  char *progargs="<dbf_file>";
  char *progver= "5.7 July 13, 1998";
  int  authorid=TRUE;
  
  
  /* Check number of arguments. */
  
  numargs = argc - 1;
  if (numargs < 1)
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
      printf("%-11s %-4c %3u %3d\n",
             fieldname,fieldtype,fieldlen,fieldnumdec);
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
  fprintf(io,"Usage: dbfitems <dbf_file>\n");
  fprintf(io,"\n");
  fprintf(io,"Lists the field definitions of a dBASE DBF file.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Arguments:\n");
  fprintf(io,"\n");
  fprintf(io,"<dbf_file> -- The dbf file to look at.\n");
  fprintf(io,"\n");
  fprintf(io,"\n");
  fprintf(io,"Notes:\n");
  fprintf(io,"\n");
  fprintf(io,"The field definitions will be listed to stdout.  This is usually the\n");
  fprintf(io,"screen.  You can use redirection to capture the data to an output\n");
  fprintf(io,"file.\n");
  fprintf(io,"\n");
  fprintf(io,"<dbf_file> must be a standard dBASE DBF file.  Do not convert it unix\n");
  fprintf(io,"format with dos2unix or d2u.\n");
  fprintf(io,"\n");
  fprintf(io,"See also dbflist and dbflook.\n");
  /* @ end help */
  return 1;
}
