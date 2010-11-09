/* infox.h
   ------------------------------------------------------------------------
   General include file for INFO stuff.
   
   This should be included by any programs that call functions from
   infolib.a.
   
   Written: 10/01/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   06/17/94 Increased INFO_MAXITEMS from 512, which was a guess, to
   2048, which should be correct.  The total length of all item names
   (including redefined items) is limited to 4096.  Since, with 2
   character item names, it would be possible to construct 4624 unique
   item names, the actual maximum number of items is given by dividing
   4096 by 2.

   06/20/94 Added union for redefined items.
   
   07/30/94 Moved union from here to the functions that uses it.  This
   is for compatibility with compilers that don't allow public unions to
   appear in more than one function in a library.

   08/12/94 Incorporated Todd Stellhorn's direct INFO access functions and
   modified some of mine to call his.
   
   ------------------------------------------------------------------------ */

/* Declare a public union so we can address various temporary variables
   from the same space. */

union    infounion
{
  char   charbuff[32];
  short  shortbuff;
  int    intbuff;
  long   lintbuff;
  float  floatbuff;
  double doublebuff;
};

/* INFO parameters. */

#define INFO_CENTURY    1900  /* Used as default when creating date fields. */
#define INFO_RECLEN       82  /* Maximum record length of e00 file. */
#define INFO_NAMELEN      17  /* Length of field name buffer. */
#define INFO_MAXLEN     4096  /* Maximum record length of info data file. */
#define INFO_MAXITEMS   2048  /* Maximum number of items in an info file. */

/* Define a structure for the dbf field defs template.  This is the same
   as the dbfField structure in dbf.h. */

typedef struct
{
  char           fieldname[11];
  char           fieldtype;
  unsigned short fieldlen;
  char           fieldnumdec;
} dbfTemplate;


/* Status return codes for infoerror or infoderror. */

#define INFO_OK                1

#define INFO_READ_HEAD        -100
#define INFO_SHORT_HEAD       -101
#define INFO_WRITE_HEAD       -102
#define INFO_NO_ITEMS         -103
#define INFO_NO_RECLEN        -104
#define INFO_NO_RECORDS       -105

#define INFO_READ_DEFS        -110
#define INFO_TOO_MANY_ITEMS   -111
#define INFO_NO_FIELDS        -112
#define INFO_SHORT_DEF        -113
#define INFO_BAD_ILEN         -114
#define INFO_BAD_COLUMN       -115
#define INFO_BAD_OLEN         -116
#define INFO_BAD_ITYPE        -117
#define INFO_BAD_NUMDEC       -118
#define INFO_BAD_IW_F         -119
#define INFO_BAD_IW_B         -120
#define INFO_DUP_NAME         -121
#define INFO_DATA_NOT_INT     -135
#define INFO_DATA_NOT_NUM     -122
#define INFO_INT_TOO_WIDE     -141
#define INFO_NUM_TOO_WIDE     -123
#define INFO_FLOAT_TOO_WIDE   -124
#define INFO_BAD_DBVER        -125
#define INFO_TOO_MANY_DECS    -126
#define INFO_BAD_DEF          -127
#define INFO_BAD_IW_D         -128
#define INFO_BAD_OW_D         -129
#define INFO_WRITE_DEFS       -130
#define INFO_ITEM_NOT_FOUND   -131
#define INFO_BAD_DATE         -138
#define INFO_BAD_SIZEOF       -140
#define INFO_REC_TOO_LONG     -142
#define INFO_READ_DATA        -150
#define INFO_WRITE_DATA       -160

#define ASCII_READ_DATA       -132
#define ASCII_BAD_IFMT        -133
#define ASCII_NULL_FIELD      -134
#define ASCII_FLOATING_N      -136
#define ASCII_DATE_MISMATCH   -137
#define ASCII_BAD_QUOTE       -139

#define INFO_OPEN_READ        -201
#define INFO_OPEN_WRITE       -202
#define INFO_READ_TEMPLATE    -203
#define INFO_READ_RECORD      -204
#define INFO_DECODE_RECORD    -205
#define INFO_DEFINE_ITEM      -206
#define INFO_ENCODE_RECORD    -207
#define INFO_WRITE_RECORD     -208
#define INFO_MALLOC           -209
#define INFO_MALLOC_FIELD     -210
#define INFO_MALLOC_DBF       -211


/* Function prototypes. */

int           headinfo2dbf();
dbfTemplate  *defsinfo2dbf();
TemplateList *defsdbf2info();
int           recinfo2dbf();
int           recdbf2info();
int           recasc2info();
int           recinfo2asc();
TemplateList *ascgetdefs();
int           ascputdefs();
int           infoerror();
int           infoderror();
short int     getnumredef();
TemplateList *getitems();
int           putitems();
