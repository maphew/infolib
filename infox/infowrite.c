/* infowrite.c
   ------------------------------------------------------------------------
   Reads an ASCII file and writes INFO data file of it.
   This is just for testing infoxlib stuff.  See also inforead.
   
   Written: 08/16/94 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rbdpaths.h>
#include MISC_H
#include INFOLIB_H
#include INFO_H
#include INFODEFS_H
#include INFOSWAP_H

#define TRUE 1
#define FALSE 0
#undef DEBUGGING

int main(argc, argv)
     int    argc;
     char   *argv[];
{
  int        numargs, done, ok, status, pos;
  short int  i;
  FILE       *ioin, *ioout;
  char       ifile[80];
  char       temp80[80];
  long       recnum, itemnum;
  short int  itemstarts[INFO_MAXITEMS], itemolens[INFO_MAXITEMS];
  
  /* These are for the INFO file. */
  
  char           *InfoFilePath;
  InfoFile       *FilePtr;
  TemplateList   *Template;
  long int        reclen, numrecs;
  short int       numitems, numredefitems;
  short int       itemstart, itemtype, itemilen, itemolen, itemnumdec;
  char            itemname[INFO_NAMELEN];
#ifdef DEBUGGING
  char            numdecbuff[3];
  char           *itemtypes="_DCINBFLPVOT";
  char            typebuff;
#endif
  
  /* These are for the ASCII file. */
  
  char            irec[(INFO_MAXLEN*2)+1];
  
  
  /* Set program details. */
  
  char *progname="INFOWRITE";
  char *progargs="<in_file> <info_file>";
  char *progver= "1.3 January 17, 1995";
  int  authorid=FALSE;
  
  
  /* Check number of arguments. */
  
  numargs = argc - 1;
  if (numargs < 2 || numargs > 2)
    {
      usage(progname,progver,progargs,authorid);
      exit(0);
    }
  

  /* Fetch arguments, check validity. */

  strcpy(ifile,argv[1]);
  InfoFilePath=argv[2];

  
  /* Delete existing output file. */
  
  if (InfoFileExists(InfoFilePath))
    {
      if (!InfoDeleteFile(InfoFilePath))
        {
          printf("%s: Could not delete existing INFO file \"%s\".\n",progname,
                 InfoFilePath);
        }
      else
        {
          i=0; /* dummy action */
#ifdef DEBUGGING
          printf("%s: Delete pre-existing output info file ok.\n",
                 progname);
#endif
        }
    }
  else
    {
      i=0; /* dummy action */
#ifdef DEBUGGING
      printf("%s: Output info file does not already exist.\n",progname);
#endif
    }
  
  
  /* Create an empty output INFO file. */
  
  Template = (TemplateList *) NULL;
  status=InfoCreateFile(InfoFilePath,Template);
  if (status != INFO_OK)
    {
      printf("%s: Could not create output file \"%s\".\n",progname,
             InfoFilePath);
      exit(0);
    }
#ifdef DEBUGGING
  printf("%s: Create empty output info file ok.\n",progname);
#endif
  
  
  /* Open the input file. */
  
  if ((ioin = fopen(ifile,"r")) == NULL)
    {
      printf("%s: Could not open input file \"%s\".\n",progname,ifile);
      exit(0);
    }
#ifdef DEBUGGING
  printf("%s: Open empty output info file ok.\n",progname);
#endif

  
  /* Fetch header terms. */
  
  fgets(temp80,80,ioin);
  reclen=atoi(temp80);
  fgets(temp80,80,ioin);
  numrecs=atoi(temp80);
  fgets(temp80,80,ioin);
  numitems=atoi(temp80);
  fgets(temp80,80,ioin);
  numredefitems=atoi(temp80);

  
  /* Read item definitions. */

  itemstart=1;
  for (itemnum=0; itemnum < numitems; itemnum++)
    {
      
      /* Read an item def from the ascii file. */
      
      fgets(irec,80,ioin);
      strext(irec,2,itemname,' ');
      strpad(itemname,' ',16);
      strext(irec,3,temp80,' ');
      itemilen=atoi(temp80);
      strext(irec,4,temp80,' ');
      itemolen=atoi(temp80);
      strext(irec,5,temp80,' ');
      itemtype=atoi(temp80);
      strext(irec,6,temp80,' ');
      itemnumdec=atoi(temp80);

      /* Populate item start and item width lists. */
      
      itemstarts[itemnum]=itemstart;
      itemstart=itemstart+itemolen;
      itemolens[itemnum]=itemolen;
      
      /* Define the item in the INFO file. */
      
#ifdef DEBUGGING
      typebuff=itemtypes[itemtype];
      if (itemnumdec < 0)
        strcpy (numdecbuff,"  ");
      else
        sprintf(numdecbuff,"%2d",itemnumdec);
      printf("%-16s %4d %4d %c %s\n",
              itemname,itemilen,itemolen,typebuff,numdecbuff);
#endif
      
      status=InfoDefineItem(InfoFilePath,itemname,itemilen,itemolen,
                            itemtype,itemnumdec);
      if (status != INFO_OK)
        {
          infoerror(progname,INFO_DEFINE_ITEM,ioin,ioout);
#ifdef DEBUGGING
          printf("%s: InfoDefineItem failed.\n",progname);
#endif
          exit(0);
        }
      
      /* Next item def. */
      
    }
#ifdef DEBUGGING
  printf("%s: Define items ok.\n",progname);
#endif
  
  
  /* Get the item template for the INFO file we just created. */
  
  if((FilePtr = InfoOpenFile(InfoFilePath,InfoREAD)) == (InfoFile *) NULL)
    {
      infoerror(progname,INFO_OPEN_READ,ioin,ioout);
      exit(0);
    }
  
  /* Normally we would feed numredefitems to getitems.  For this program,
     we don't want redef items, so we pass zero. */
  
  if ((Template = getitems(FilePtr,0)) == (TemplateList *) NULL)
    {
      infoerror(progname,INFO_READ_TEMPLATE,ioin,ioout);
      exit(0);
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
      exit(0);
    }
  
  
  /* Write the data section from the ascii file to the INFO file. */

  for (recnum=1; recnum <= numrecs; recnum++)
    {
      
      /* Read a data record from the input file. */
      
      fgets(irec,(INFO_MAXLEN*2)+2,ioin);
      irec[strlen(irec)-1] = 0;
      
      /* Convert fixed width ascii record to INFO internal record. */
      
      status=encoderecord(irec,FilePtr,Template,numitems,itemstarts,itemolens);
      if (status != INFO_OK)
        {
          infoderror(progname,status,FilePtr,ioin);
          exit(0);
        }
      
      
      /* Write the record to the INFO file. */
      
      if(!InfoWriteRecord(FilePtr,recnum))
        {
          infoderror(progname,INFO_WRITE_RECORD,FilePtr,ioin);
          exit(0);
        }
      
      
      /* Next record. */
      
    }
#ifdef DEBUGGING
  printf("%s: Write data section ok.\n",progname);
#endif
  
  
  /* Close files. */
  
  if(!InfoCloseFile(FilePtr))
    {
      printf("%s: Could not close output file \"%s\".\n",progname,InfoFilePath);
      exit(0);
    }
  if (fclose(ioin) != 0)
    {
      printf("%s: Could not close input file \"%s\".\n",progname,ifile);
      exit(0);
    }
  
  
  /* All done. */
  
  fflush(stdout);
  return TRUE;
}





/* encoderecord.c
   ------------------------------------------------------------------------
   Converts ascii fixed width data record to INFO internal format.
   
   Written: 08/17/94 Randy Deardorff.  USEPA Seattle.
   
   ------------------------------------------------------------------------ */

int encoderecord(ascrec,ifp,tlp,numitems,itemstarts,itemwidths)
     char         *ascrec;
     InfoFile     *ifp;
     TemplateList *tlp;
     short int    numitems, *itemstarts, *itemwidths;
{
  char            vbuff[INFO_MAXLEN];
  double          dval;
  short int       status, itemnum;
  
  /* For each item from the template. */
  
  for(itemnum = 0; itemnum < numitems; itemnum++)
    {
      
      /* Extract the item value to be encoded. */

      strncpy(vbuff,&ascrec[itemstarts[itemnum] - 1],itemwidths[itemnum]);
      vbuff[itemwidths[itemnum]] = '\0';
      dval = (double) atof(vbuff);
      
      /* Encode the item value into the IOBuffer. */
      
      status=InfoEncode(ifp,&tlp[itemnum],vbuff,dval);
      if (status != INFO_OK)
        return INFO_ENCODE_RECORD;
      
    }
  return INFO_OK;
}
