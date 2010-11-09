/* dbf.h
   --------------------------------------------------------------------------
   General include file for DBF stuff.
   
   This should be included by any programs that include dbfsys.h, or call
   functions from dbflib.a.
   
   Written: 09/29/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   09/06/94 Increased dbf record length to 4096 to handle maximum sized INFO
   record.  Actual limit for dBASE III+ is 4001.  FoxPro 2.5 is 65500.
   
   1/21/95 Added dbfField structure.
   --------------------------------------------------------------------------*/

/* DBF parameters. */

#define DBF_CENTURY    1900  /* Used as default when creating date fields. */
#define DBF_MAXLEN     4096  /* Maximum record length. */
#define DBF_NAMELEN      11  /* Length of field name buffer. */
#define DBF3_MAXFIELDS  128  /* According to dBASE III+ manual page U1-1. */
#define DBF4_MAXFIELDS  255  /* According to dBASE IV change notes p 1-4. */
#define DBF_MAXFIELDS   255  /* Maximum number of fields regardless of ver. */
#define DBF_BUFFSIZE     32  /* Size of header and field def records. */
#define DBF_END_OF_DEFS  13  /* Marks end of field definition section. */


/* Define a structure for the field defs template.  This is the structure we
   use to store the field defs AFTER they are byte swapped (if necessary),
   NOT for dbf file io.  See dbfsys.h for those. */

typedef struct
{
  char           fieldname[DBF_NAMELEN];
  char           fieldtype;
  unsigned short fieldlen;
  char           fieldnumdec;
} dbfField;


/* Status return codes. */

#define DBF_OK                  1

#define DBF_READ_HEAD          -101
#define DBF_BAD_DBFID          -102
#define DBF_WRITE_HEAD         -103

#define DBF_READ_DEFS          -111
#define DBF_TOO_MANY_FIELDS    -112
#define DBF_NO_FIELDS          -113
#define DBF_BAD_EODEFS         -114
#define DBF_WRITE_DEFS         -115
#define DBF_BAD_ITYPE          -116
#define DBF_CANNOT_DO_MEMO     -117
#define DBF_BAD_INT_WIDTH      -118
#define DBF_BAD_OFFSET         -119
#define DBF_FLOATING_N         -120
#define DBF_READ_DATA          -121
#define DBF_UNPRINT_DATA       -122
#define DBF_WRITE_DATA         -125
#define DBF_INT_EXP            -126
#define DBF_INT_REAL           -127
#define DBF_INT_JUNK           -128
#define DBF_REC_DELETED        -129
#define DBF_ALL_DELETED        -130
#define DBF_BAD_SIZEOF         -131
#define DBF_REC_TOO_LONG       -132
#define DBF_TOO_WIDE_FOR_INFO  -133
#define DBF_MALLOC_FIELD       -134
#define DBF_MALLOC             -135

#define DBF_GET_DATE           -150

/* Function prototypes. */

int dbferror();
int dbfgethead();
dbfField *dbfgetdefs();
int dbfgetrec();
int dbfputhead();
int dbfputdefs();
int dbfputrec();
int dbfputtail();
int dbfgetdate();
