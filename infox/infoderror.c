/* infoderror
   ------------------------------------------------------------------------
   A function in infoxlib.
   
   Handles fatal errors during INFO dirrect access i/o.
   
   Written: 08/12/94 Randy Deardorff
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <rbdpaths.h>
#include MISC_H
#include INFOLIB_H
#include INFO_H

int infoderror(progname,errnum,ioin,ioout)
     char     *progname;
     int      errnum;
     InfoFile *ioin;
     FILE     *ioout;
     
{
  printf("%s: ",progname);
  switch (errnum)
    {
    case INFO_READ_TEMPLATE :
      printf("Error reading INFO file item definitions.");
      break;
    case INFO_READ_RECORD :
      printf("Error reading data record from INFO file.");
      break;
    case INFO_DECODE_RECORD :
      printf("Error decoding INFO data record.");
      break;
    case INFO_BAD_ITYPE :
      printf("Bad item type in item definition.");
      break;
    case INFO_ENCODE_RECORD :
      printf("Error encoding INFO data record.");
      break;
    case INFO_WRITE_RECORD :
      printf("Error writing data record to INFO file.");
      break;
    case INFO_ITEM_NOT_FOUND :
      printf("Item not found.");
      break;
    case INFO_MALLOC :
      printf("Unable to allocate memory required to run.");
      break;
    default:
      printf("Fatal error.");              
      break;
    }
  putchar('\n');
  InfoCloseFile(ioin);
  fclose(ioout);
  return 1;
}
