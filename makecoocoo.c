/* makecoocoo.c
   ------------------------------------------------------------------------
   
   Converts a coordinate list created by dumping the coordinates to an ascii
   file via info2ascii, comma delimited and unquoted.
   
   Output file has just the coordinates, separated by a space.
   
   This is part of info2cover, an AML tool.
   
   Written: 08/10/92 Randy Deardorff, EPA Region 10, Seattle.
   
   History: (All changes are by the author unless otherwise noted)
   
   04/23/93 Added logic to handle loose dms coords with d m s values
   separated by spaces or colons.  Formerly insisted that coords be
   packed.  Changed makerawcoo.aml so that the file we're reading here is
   now comma delimited (was space before).  Also now there is no header
   record to skip and no record number prepended to each record.
   
   07/13/93 Eliminated makerawcoo.aml in favor of info2ascii.
   If coorinates are blank, convert to zero.
   
   02/28/95 Added record number to error messages.
   04/20/95 Fixed bug in maplib which caused bogus results of this program.
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include MAP_H

#define TRUE 1
#define FALSE 0
#define MAXLEN 82  /* effective max reclen is MAXLEN-2 */
#undef DEBUGGING

int main(argc, argv)
     int    argc;
     char   *argv[];
{
  int    numargs, openin, openout, dodms, okx, oky, ok, recnum;
  char   ifile[80], ofile[80], errfile[80], inunits[12];
  char   chx[40], chy[40];
  char   irec[MAXLEN+1], orec[MAXLEN+1], curitem[MAXLEN+1];
  double x, y, sec;
  int    deg, min;
  FILE   *ioin, *ioout;
  
  /* Set program details. */
  
  char *progname="MAKECOOCOO";
  char *progargs="<infile> <outfile> <DMS | OTHER> <errorfile>";
  char *progver= "5.7 July 13, 1998";
  int  authorid=TRUE;
  
  /* Init program vriables. */
  
  openin=FALSE;
  openout=FALSE;
  
  /* Check arguments. */
  
  numargs = argc - 1;
  
  /* Check number of arguments. */
  
  if (numargs != 4)
    {
      usage(progname,progver,progargs,authorid);
      return 0;
    }
  
  /* Fetch arguments, check validity, and set switches based on them. */
  
  strcpy(ifile,argv[1]);
  strcpy(ofile,argv[2]);
  strcpy(inunits,argv[3]);
  strcpy(errfile,argv[4]);

  recnum=0;
  if (strcmp(ofile,ifile) == 0)
    {
      uhoh("Input and output filenames must be different.",
           ofile,errfile,ioin,ioout,openin,openout,recnum);
      return 0;
    }
  
  strupr(inunits);
  dodms=(strcmp(inunits,"DMS") == 0);
  
  /* Print arguments */
  
#ifdef DEBUGGING
  printf("   Input file=%s\n",ifile);
  printf("  Output file=%s\n",ofile);
  printf("   Error file=%s\n",errfile);
  printf("  Input units=%s",inunits);
  if (dodms)
    printf(" (DMS)\n");
  else
    printf(" (non DMS)\n");
#endif
  
  /* If the error file exists, delete it! */
  
  if (sysexist(errfile))
    sysdel(errfile);
  
  /* Open the input file for reading. */
  
  if ((ioin = fopen(ifile, "r")) == NULL)
    {
      uhoh("Could not open input file.",
           ofile,errfile,ioin,ioout,openin,openout,recnum);
      return 0;
    }
  openin=TRUE;
  
  /* Open output file for writing. */
  
  if ((ioout = fopen(ofile, "w")) == NULL)
    {
      uhoh("Could not create output file.",
           ofile,errfile,ioin,ioout,openin,openout,recnum);
      return 0;
    }
  openout=TRUE;
  
  /* Loop through input file until end. */
  
  while (fgets(irec, MAXLEN, ioin) != NULL)
    {
      irec[strlen(irec)-1] = 0;        /* change LF to NULL */
      recnum++;
      
      /* Make sure two items present. */
      
      if (stritems(irec,',') != 2)
        {
          uhoh("A record contains other than 2 items",
               ofile,errfile,ioin,ioout,openin,openout,recnum);
          return 0;
        }
      
      /* Extract X and Y, and strip off any leading and trailing spaces. */
      
      strext(irec,1,chx,',');
      strtrim(chx,"both",' ');
      strext(irec,2,chy,',');
      strtrim(chy,"both",' ');
      
      /* If the coordinate is blank then set it to zero. */
      
      if (strnull(chx))
        strcpy(chx,"0");
      if (strnull(chy))
        strcpy(chy,"0");
      
      /* If input units are DMS, unpack to format that project likes.
         Note that it doesn't matter if the coords are already unpacked,
         or whether the components are separated by spaces or colons. */
      
      if (dodms)
        {        
          okx=dmsunpack(chx,' ');
          oky=dmsunpack(chy,' ');
          if (!(okx) || !(oky))
            {
              uhoh("Bad DMS coordinates in input file",
                   ofile,errfile,ioin,ioout,openin,openout,recnum);
              return 0;
            }
        }
      
      /* Build output record. */
      
      sprintf(orec,"%s %s",chx,chy);
      
      /* Write output to file. */
      
      fprintf(ioout,"%s\n",orec);
    }
  
  /* End of file reached.  Close files. */
  
  fclose(ioout);
  openout=FALSE;
  fclose(ioin);
  openin=FALSE;
  
  /* All done. */
  
  return 1;
}


/* uhoh
   ------------------------------------------------------------------------
   Closes input and out put files if they are open.
   Deletes the output file if it exists.
   Writes a message to the error file.
   ------------------------------------------------------------------------ */

int uhoh(msg,ofile,errfile,ioin,ioout,openin,openout,recnum)
     char   *msg, *ofile, *errfile;
     FILE   *ioin, *ioout;
     int    openin, openout, recnum;
{
  if (openin) fclose(ioin);
  if (openout) fclose(ioout);
  
  /* If the output file exists, delete it! */
  
  if (sysexist(ofile))
    sysdel(ofile);
  
  /* Write message to errror file. */
  
  if ((ioout = fopen(errfile, "w")) != NULL)
    {
      if (recnum > 0) 
        fprintf(ioout,"%s at record %d.\n",msg,recnum);
      else
        fprintf(ioout,"%s\n",msg);
      fclose(ioout);
    }
  return 1;
}
