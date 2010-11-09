/* dbfsys.h
   --------------------------------------------------------------------------
   Special include file for DBF stuff.
   
   Declares the structures and unions needed to read/write dbf files.
   
   This should be included by any functions or programs that access
   the structures directly.  Programs that access the structures
   indirectly by calling the put and get functions from dbflib.a do
   not need to include this header.
   
   Written: 09/25/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)

   07/20/94 Changed DBF_HEAD and DBF_REC to be all character and added
   swapunion.  This permits handling various sizeofs for other
   systems.

   07/30/94 Moved unions from here to the functions that use them.  This
   is for compatibility with compilers that don't allow public unions to
   appear in more than one function in a library.
   
   -------------------------------------------------------------------------- */

/* Define a (byte swapable) structure for dbf file's header. */

typedef struct
{
  unsigned char dbf_id;
  unsigned char last_update[3];
  unsigned char last_rec[4];
  unsigned char data_offset[2];
  unsigned char rec_size[2];
  unsigned char filler[20];
} DBF_HEAD;


/* Define a (byte swapable) structure for the dbf file's field defs.
   This is the structure we use to put/get field defs to/from the dbf file. */

typedef struct
{
  char field_name[DBF_NAMELEN];
  char field_type;
  char dummy[4];
  union
    {
      unsigned char char_len[2];
      struct
        {
          char len;
          char dec;
        } num_size;
    } len_info;
  char filler[14];
} FIELD_REC;


/* Union the byte swapable dbf structures to character arrays.
   This makes the structures byte addressable for byte swapping. */

union    header
{
  DBF_HEAD headst;
  char     headch[32];
};

union    fieldrec
{
  FIELD_REC fieldst;
  char      fieldch[32];
};


/* Declare a public union so we can address various temporary variables
   from the same space. */

union    dbfunion
{
  char           charbuff[4];
  unsigned short ushortbuff;
  unsigned       uintbuff;
  int            intbuff;
  long           lintbuff;
};
