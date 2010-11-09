/* makegencoo.c
   -----------------------------------------------------------------------
   
   Reads a file consisting of a list of coordinates, separated by a
   space (such as is suitable for ARC/INFO's PROJECT FILE command) and
   creates a file suitable for using as an input file for ARC/INFO's
   GENERATE command, i.e., a list of numbered coordinates, with the
   numbers, and the coordinates separated by commas, with a "END" to
   mark the end of the file.
   
   Input file has just the coordinates, separated by a space.
   
   Output file has record number (user-id), x, and y, all separated by
   commas.
   
   This is part of info2cover, an AML tool.
   
   Written: 08/10/92 Randy Deardorff, EPA Region 10, Seattle.
   
   History: (All changes are by the author unless otherwise noted)
   
   04/23/93 Added new usage.

   03/01/95 Added line number to error message.
   
   ----------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H

#define TRUE 1
#define FALSE 0
#define MAXLEN 82  /* effective max reclen is MAXLEN-2 */
#undef DEBUGGING

int main(argc, argv)
     int    argc;
     char   *argv[];
{
  int    numargs, openin, openout, recnum;
  char   ifile[80], ofile[80], errfile[80], x[40], y[40];
  char   irec[MAXLEN+1], orec[MAXLEN+1], curitem[MAXLEN+1];
  FILE   *ioin, *ioout;
  
  /* Set program details. */
  
  char *progname="MAKEGENCOO";
  char *progargs="<infile> <outfile> <errorfile>";
  char *progver= "5.7 July 13, 1998";
  int  authorid=TRUE;
  
  /* Init program vriables. */
  
  openin=FALSE;
  openout=FALSE;
  recnum=0;
  
  /* Check arguments. */
  
  numargs = argc - 1;
  
  /* Check number of arguments. */
  
  if (numargs != 3)
    {
      usage(progname,progver,progargs,authorid);
      return 0;
    }
  
  /* Fetch arguments, check validity, and set switches based on them. */
  
  strcpy(ifile,argv[1]);
  strcpy(ofile,argv[2]);
  strcpy(errfile,argv[3]);
  
  if (strcmp(ofile,ifile) == 0)
    {
      uhoh("Input and output filenames must be different.",
           ofile,errfile,ioin,ioout,openin,openout,recnum);
      return 0;
    }
  
  /* Print arguments */
  
#ifdef DEBUGGING
  printf("   Input file=%s\n",ifile);
  printf("  Output file=%s\n",ofile);
  printf("   Error file=%s\n",errfile);
#endif
  
  
  /* If the error file exists, delete it! */
  
  if (sysstat(errfile,"exists"))
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
      
      if (stritems(irec,' ') != 2)
        {
          uhoh("DMS?  If so, must project.  Other than two coordinates",
               ofile,errfile,ioin,ioout,openin,openout,recnum);
          return 0;
        }
      
      /* Extract X and Y, and strip off any leading or training spaces. */
      
      strext(irec,1,x,' ');
      strtrim(x,"both",' ');
      strext(irec,2,y,' ');
      strtrim(y,"both",' ');
      
      /* Make sure both coordinates are numbers. */
      
      if (!(strisnum(x) && strisnum(y)))
        {
          uhoh("DMS?  If so, must project.  Non numeric coordinate",
               ofile,errfile,ioin,ioout,openin,openout,recnum);
          return 0;
        }
      
      /* Build output record. */
      
      sprintf(orec,"%d,%s,%s",recnum,x,y);
      
      /* Write output to file. */
      
      fprintf(ioout,"%s\n",orec);
    }
  
  /* End of file reached.  Write "END". */
  
  fprintf(ioout,"END\n");
  
  /* Close files. */
  
  fclose(ioout);
  openout=FALSE;
  fclose(ioin);
  openin=FALSE;
  
  /* All done. */
  
  return 1;
}


/* uhoh
   -----------------------------------------------------------------------
   Closes input and out put files if they are open.
   Deletes the output file if it exists.
   Writes a message to the error file.
   ----------------------------------------------------------------------- */

int uhoh(msg,ofile,errfile,ioin,ioout,openin,openout,recnum)
     char   *msg, *ofile, *errfile;
     FILE   *ioin, *ioout;
     int    openin, openout, recnum;
{
  if (openin) fclose(ioin);
  if (openout) fclose(ioout);

  
  /* If the output file exists, delete it! */
  
  if (sysstat(ofile,"exists"))
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
