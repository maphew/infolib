/* dbferror
   ------------------------------------------------------------------------
   A function in dbflib.

   Handles fatal errors for dbf stuff while files are open.
   
   Written: 09/29/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <rbdpaths.h>
#include MISC_H
#include DBF_H

int dbferror(progname,errnum,ioin,ioout)
     char *progname;
     int  errnum;
     FILE *ioin, *ioout;
{
  printf("%s: ",progname);
  switch (errnum)
    {
      
    case DBF_READ_HEAD :
      printf("Could not read dbf header.");
      break;
    case DBF_WRITE_HEAD :
      printf("Could not write dbf header.");
      break;
    case DBF_BAD_DBFID :
      printf("Unrecognized dbf id.");
      break;
    case DBF_READ_DEFS :
      printf("Error reading field definition from dbf file.");
      break;
    case DBF_WRITE_DEFS :
      printf("Error writing field definition to dbf file.");
      break;
    case DBF_TOO_MANY_FIELDS :
      printf("Too many field definitions.");
      break;
    case DBF_BAD_EODEFS :
      printf("Bad end of field definitions marker.");
      break;
    case DBF_BAD_OFFSET :
      printf("Bad data offset.");
      break;
    case DBF_FLOATING_N :
      printf("Type N value floats beyond its defined field width.");
      break;
    case DBF_READ_DATA :
      printf("Error reading data section of dbf file.");
      break;
    case DBF_WRITE_DATA :
      printf("Error writing data section of dbf file.");
      break;
    case DBF_GET_DATE :
      printf("Unable to get system date.");
      break;
    case DBF_BAD_ITYPE :
      printf("Unrecognized field type in dbf file.");
      break;
    case DBF_CANNOT_DO_MEMO :
      printf("Unable to process dbf files with memo fields.");
      break;
    case DBF_BAD_INT_WIDTH :
      printf("Type N field conversion error.  Bad internal width.");
      break;
    case DBF_UNPRINT_DATA :
      printf("Data section contains non-printable characters.");
      break;
    case DBF_INT_EXP:
      printf("Non-decimal N or F field contains an exponential value.");
      break;
    case DBF_INT_REAL:
      printf("Non-decimal N or F field contains a real value.");
      break;
    case DBF_INT_JUNK:
      printf("Bad value in a non-decimal N or F field.");
      break;
    case DBF_ALL_DELETED:
      printf("All data records are flagged for delete.");
      break;
    case DBF_REC_TOO_LONG:
      printf("Record length exceeds maximum supported length (%d).",DBF_MAXLEN);
      break;
    case DBF_TOO_WIDE_FOR_INFO :
      printf("Record too wide for INFO.");
      break;
    case DBF_MALLOC :
      printf("Unable to allocate memory required to run.");
      break;
    case DBF_MALLOC_FIELD :
      printf("Unable to allocate enough memory for the field definitions.");
      break;
    case DBF_BAD_SIZEOF:
      printf("Cannot run on ths system due to incompatible data types.\n");
      printf("Size of unsigned short=%d.\n",sizeof(unsigned short));
      printf("Size of unsigned=%d.\n",sizeof(unsigned));
      printf("Size of int=%d.\n",sizeof(int));
      printf("Size of long=%d.\n",sizeof(long));
      printf("Size of float=%d.\n",sizeof(float));
      printf("Size of double=%d.\n",sizeof(double));
      printf("Please report this error to the program author.\n");
      usage("x","x","x",2);
      break;
    default:
      printf("Fatal error.");              
      break;
    }
  putchar('\n');
  fclose(ioin);
  fclose(ioout);
  return 1;
}
