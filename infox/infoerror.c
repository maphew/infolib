/* infoerror
   ------------------------------------------------------------------------
   A function in infoxlib.

   Handles fatal errors for info stuff while files are open.
   
   Written: 10/01/92 Randy Deardorff
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   ------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <rbdpaths.h>
#include MISC_H
#include INFOLIB_H
#include INFO_H

int infoerror(progname,errnum,ioin,ioout)
     char *progname;
     int  errnum;
     FILE *ioin, *ioout;
{
  printf("%s: ",progname);
  switch (errnum)
    {
    case INFO_READ_HEAD :
      printf("Could not read info header.");
      break;
    case INFO_WRITE_HEAD :
      printf("Could not write info header.");
      break;
    case INFO_SHORT_HEAD :
      printf("Header record of improper length.");
      break;
    case INFO_NO_RECLEN :
      printf("Error decoding record length.");
      break;
    case INFO_NO_RECORDS :
      printf("Error decoding number of records.  Is the file empty?");
      break;
    case INFO_READ_DEFS :
      printf("Error reading item definition from info file.");
      break;
    case INFO_WRITE_DEFS :
      printf("Error writing item definition to info file.");
      break;
    case INFO_TOO_MANY_ITEMS :
      printf("Too many item definitions.");
      break;
    case INFO_SHORT_DEF :
      printf("Item definition record of improper length.");
      break;
    case INFO_BAD_DEF :
      printf("Bad item definition.");
      break;
    case INFO_NO_ITEMS :
      printf("No item definitions.");
      break;
    case INFO_ITEM_NOT_FOUND :
      printf("Item not found.");
      break;
    case INFO_BAD_ILEN :
      printf("Bad internal length in item definition.");
      break;
    case INFO_BAD_COLUMN :
      printf("Bad starting column in item definition.");
      break;
    case INFO_BAD_OLEN :
      printf("Bad output length in item definition.");
      break;
    case INFO_BAD_ITYPE :
      printf("Bad item type in item definition.");
      break;
    case INFO_BAD_IW_D :
      printf("Bad item width for date item.");
      break;
    case INFO_BAD_OW_D :
      printf("Bad output width for date item.");
      break;
    case INFO_BAD_IW_B :
      printf("Illegal item width for binary item.");
      break;
    case INFO_BAD_IW_F :
      printf("Illegal item width for floating point item.");
      break;
    case INFO_DUP_NAME :
      printf("Duplicate field name.");
      break;
    case INFO_BAD_NUMDEC :
      printf("Bad number of decimals in item definition.");
      break;
    case INFO_READ_DATA :
      printf("Error reading data section of info file.");
      break;
    case INFO_DATA_NOT_INT :
      printf("Non-integer data in integer or binary field.");
      break;
    case INFO_DATA_NOT_NUM :
      printf("Numeric or float field contains non-numeric data.");
      break;
    case INFO_INT_TOO_WIDE :
      printf("Integer data too wide for field definition.");
      break;
    case INFO_NUM_TOO_WIDE :
      printf("Numeric data too wide for field definition.");
      break;
    case INFO_FLOAT_TOO_WIDE :
      printf("Float data too wide for field definition.");
      break;
    case INFO_TOO_MANY_DECS :
      printf("Numeric or float item has too many decimals.");
      break;
    case INFO_BAD_DBVER :
      printf("Unsupported dbase version number.");
      break;
    case INFO_BAD_DATE :
      printf("Invalid date.");
      break;
    case INFO_WRITE_DATA :
      printf("Error writing data section of info file.");
      break;
    case ASCII_READ_DATA :
      printf("Error reading ascii data file.");
      break;
    case ASCII_BAD_IFMT :
      printf("Unknown ascii file format.");
      break;
    case ASCII_DATE_MISMATCH :
      printf("Date item definition disagrees with date format.");
      break;
    case ASCII_NULL_FIELD :
      printf("Empty field in ascii data file.");
      break;
    case ASCII_FLOATING_N :
      printf("Type N value floats beyond its defined item width.");
      break;
    case ASCII_BAD_QUOTE :
      printf("Mismatched quotes.");
      break;
    case INFO_DEFINE_ITEM :
      printf("Error defining item.");
      break;
    case INFO_OPEN_WRITE :
      printf("Error opening INFO file for write access.");
      break;
    case INFO_OPEN_READ :
      printf("Error opening INFO file for read access.");
      break;
    case INFO_READ_TEMPLATE :
      printf("Error reading INFO file template.");
      break;
    case INFO_MALLOC_FIELD :
      printf("Unable to allocate enough memory for the INFO item definitions.");
      break;
    case INFO_MALLOC_DBF :
      printf("Unable to allocate enough memory for the dbf field definitions.");
      break;
    case INFO_REC_TOO_LONG :
      printf("Record length exceeds maximum (%d).",INFO_MAXLEN);
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
