/* swaptest.c
   ------------------------------------------------------------------------
   Tests whether byte swapping is required for the dbf stuff.
   Stand alone code.  No non-standard libraries or headers required.
   
   To compile:
   cc -o swaptest swaptest.c
   
   Written: 05/25/94 Randy Deardorff.  USEPA Seattle.
   
   10/04/94 Allow for variously sized data types.  Previous version
   failed unless long was 4 bytes.

   04/27/95 Added some type casts to eliminate nuisance compile warnings.
   
   --------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TRUE 1
#define FALSE 0
#undef DEBUGGING


/* Define the structure of the dbf file's header. */

typedef struct
{
  unsigned char dbf_id;
  unsigned char last_update[3];
  unsigned char last_rec[4];
  unsigned char data_offset[2];
  unsigned char rec_size[2];
  unsigned char filler[20];
} DBF_HEAD;


/* Union the dbf header structure to a character array.
   This makes the structure byte addressable for byte swapping. */

union    header
{
  DBF_HEAD headst;
  char     headch[32];
};
union    header head;


/* Declare a union so we can address various temporary variables
   from the same space. */

union    dbfunion
{
  char           charbuff[4];
  unsigned short ushortbuff;
  unsigned       uintbuff;
  int            intbuff;
  long           lintbuff;
};
union dbfunion dbfswap;


/* Main program. */

int main(argc, argv)
     int  argc;
     char *argv[];
{
  FILE           *ioin;
  char           ifile[80], progname[20], errmsg[80];
  unsigned char  byte;
  unsigned int   numbytes, filesize;
  int            lastrec;
  unsigned short dataoffset, recsize;
  int            numfields, swapflag, error, openflag, i, numargs;
  char *progver= "5.7 July 13, 1998";
  strcpy(progname,"SWAPTEST");
  strcpy(errmsg,"Error");
  error=FALSE;
  openflag=FALSE;

  
  /* Get arguments, if any. */
  
  numargs = argc - 1;
  if (numargs >= 1 )
    strcpy(ifile,argv[1]);
  else
    strcpy(ifile,"swaptest.dbf");
  
  
  /* Open the input file for binary reading. */

  if ((ioin = fopen(ifile, "rb")) == NULL)
    {
      printf("%s: Could not open input file \"%s\".\n",progname,ifile);
      return FALSE;
    }
  openflag=TRUE;

  
  /* Read the header section into the structure. */
  
  for (numbytes=0; numbytes < sizeof(head.headst); numbytes++)
    if (fread(&byte,sizeof(char),1,ioin) == sizeof(char))
      head.headch[numbytes]=byte;
    else
      {
        strcpy(errmsg,"Error reading header");
        error=TRUE;
        numbytes=9999;
      }

  
  /* Close file. */
  
  if (openflag)
    {
      fclose(ioin);
      openflag=FALSE;
    }

  
  /* Make sure we got the correct number of bytes. */
  
  if (numbytes != sizeof(head.headst))
    {
      strcpy(errmsg,"Incorrect header size");
      error=TRUE;
    }

  
  /* List the important part of the header to the screen. */
  
#ifdef DEBUGGING
  if (!error)
    for (i=4; i <= 11; i++)
      {
        byte=head.headch[i];
        printf("%d = %d %u %X\n",i,byte,byte,byte);
      }
#endif

  
  /* See what we have before swapping. */

  dbfswap.charbuff[0]=head.headch[10];
  dbfswap.charbuff[1]=head.headch[11];
  if (sizeof(unsigned short)==2)
    recsize=dbfswap.ushortbuff;
  else
    if (sizeof(unsigned)==2)
      recsize=dbfswap.uintbuff;
    else
      {
        printf("%s: Data type size error.\n",progname);
        return FALSE;
      }
  
  dbfswap.charbuff[0]=head.headch[8];
  dbfswap.charbuff[1]=head.headch[9];
  if (sizeof(unsigned short)==2)
    dataoffset=dbfswap.ushortbuff;
  else
    if (sizeof(unsigned)==2)
      dataoffset=dbfswap.uintbuff;
    else
      {
        printf("%s: Data type size error.\n",progname);
        return FALSE;
      }
  
  dbfswap.charbuff[0]=head.headch[4];
  dbfswap.charbuff[1]=head.headch[5];
  dbfswap.charbuff[2]=head.headch[6];
  dbfswap.charbuff[3]=head.headch[7];
  if (sizeof(long)==4)
    lastrec=dbfswap.lintbuff;
  else
    if (sizeof(int)==4)
      lastrec=dbfswap.intbuff;
    else
      {
        printf("%s: Data type size error.\n",progname);
        return FALSE;
      }
  
  filesize=(recsize * lastrec) + dataoffset + 1;
  numfields=(((int) dataoffset) - 31) / 32;
  
#ifdef DEBUGGING
  printf("Before swap:\n");
  printf("       Data offset: %u\n",dataoffset);
  printf("       Record size: %u\n",recsize);
  printf(" Number of records: %d\n",lastrec);
  printf("  Number of fields: %d\n",numfields);
  printf("         File size: %u\n",filesize);
#endif
  
  if (dataoffset==161 && recsize==35 && lastrec==3)
    {
      /* Values ok.  Swapping not required. */
      swapflag=FALSE;
    }
  else
    {
      /* Values bad.  Try swapping. */

      dbfswap.charbuff[0]=head.headch[11];
      dbfswap.charbuff[1]=head.headch[10];
      if (sizeof(unsigned short)==2)
        recsize=dbfswap.ushortbuff;
      else
        if (sizeof(unsigned)==2)
          recsize=dbfswap.uintbuff;
        else
          {
            printf("%s: Data type size error.\n",progname);
            return FALSE;
          }
      
      dbfswap.charbuff[0]=head.headch[9];
      dbfswap.charbuff[1]=head.headch[8];
      if (sizeof(unsigned short)==2)
        dataoffset=dbfswap.ushortbuff;
      else
        if (sizeof(unsigned)==2)
          dataoffset=dbfswap.uintbuff;
        else
          {
            printf("%s: Data type size error.\n",progname);
            return FALSE;
          }
      
      dbfswap.charbuff[0]=head.headch[7];
      dbfswap.charbuff[1]=head.headch[6];
      dbfswap.charbuff[2]=head.headch[5];
      dbfswap.charbuff[3]=head.headch[4];
      if (sizeof(long)==4)
        lastrec=dbfswap.lintbuff;
      else
        if (sizeof(int)==4)
          lastrec=dbfswap.intbuff;
        else
          {
            printf("%s: Data type size error.\n",progname);
            return FALSE;
          }
      
      filesize=(recsize * lastrec) + dataoffset + 1;
      numfields=(((int) dataoffset) - 31) / 32;
      
#ifdef DEBUGGING
      printf("After swap:\n");
      printf("       Data offset: %u\n",dataoffset);
      printf("       Record size: %u\n",recsize);
      printf(" Number of records: %d\n",lastrec);
      printf("  Number of fields: %d\n",numfields);
      printf("         File size: %u\n",filesize);
#endif
      
      if (dataoffset==161 && recsize==35 && lastrec==3)
        {
          /* Values are now ok.  Swapping IS required. */
          swapflag=TRUE;
        }
      else
        {
          strcpy(errmsg,"Values incorrect whether swapped or not");
          error=TRUE;
        }
    }      
  
  /* Status. */
  
  if (error)
    printf("%s: %s.\n",progname,errmsg);
  else
    if (swapflag)
      printf("%s: Byte swapping is required on this system.\n",progname);
    else
      printf("%s: Byte swapping is not required on this system.\n",progname);
  
  return (!error);
}
