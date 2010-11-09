/* %W% %G% %U%
 *  Notice:  This software is being released into the public
 *           domain.  It may be freely used whole or in part or 
 *           distributed as a whole as long as this notice remains
 *           attached to the source code.  The author and ESRI 
 *           do not assume any liability or responsibility in the 
 *           use or distribution of this software.  Direct any 
 *           questions or comments via email to the author at 
 *           tstellhorn@esri.com.
 *            
 *                              Todd Stellhorn
 *                              ESRI
 */

/*************************************************************************
  Name: infodefs.h
  
  Description: infolib INFO file definitions.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  2/94 EW - macros to deal with Arc/Info 7.0 info directory naming
  convention changes (8.3 file names)
  
  10/94 Randy Deardorff - Very minor changes to work together with my
  infox library.  Note that target os is set in rbdpaths.h, hence they
  are all commented out here.
  
  12/94 Randy Deardorff - Changed INFO_MAX_DIRSIZE to 9999 from 999.
  Not sure what the implications are since INFO_MAX_DIRSIZE is only
  used once in InfoTempName (infoname.c).
  
  1/14/95 Randy Deardorff - Changes for use on BORLAND_C Borland C++
  3.1.  Added target os definition section for BORLAND_C.  Also added
  MacroPROMOTE preprocessor constant.  This is set to 0 in the
  BORLAND_C section and 1 on all others.
  
  2/3/95 Randy Deardorff - Added a new target os section for use with
  djgpp (DJ's GNU C++ for DOS) on the IBM PC.
  
  *************************************************************************/


/***************************************************************************/

#ifndef _INFODEFS_H_
#define _INFODEFS_H_
#undef  DOS

/***************************************************************************/

/* target OS */

/* If you are using Todd's functions in conjunction with the EPA region 10
   dbf utilities, leave these all commented.  See rbdpaths.h. */

/* #define UNIX */
/* #define ULTRIX */
/* #define BORLAND_C   0 */  
/* #define DJGPP_DOS   0 */  

/***************************************************************************/

#if BORLAND_C

#include <limits.h>
#define MAXPATHLEN          128
#define INFO_SWAP

#define OPSYS_SEP            "\\"
#define DIR_EXT             "dr9"
#define DIR83_EXT           "dir"
#define DATA_EXT            "dat"
#define NIT_EXT             "nit"

#define PATH_DELIMITER1       ':'
#define PATH_DELIMITER2       '!'

#define PATH_SIZE      MAXPATHLEN         
#define MacroPROMOTE            0
#define UseSTRICMP              0
#define DOS

#endif  /* BORLAND_C */

/***************************************************************************/

#if DJGPP_DOS

#include <sys/param.h>
#define INFO_SWAP

#define OPSYS_SEP            "\\"
#define DIR_EXT             "dr9"
#define DIR83_EXT           "dir"
#define DATA_EXT            "dat"
#define NIT_EXT             "nit"

#define PATH_DELIMITER1       ':'
#define PATH_DELIMITER2       '!'

#define PATH_SIZE      MAXPATHLEN         
#define MacroPROMOTE            1
#define UseSTRICMP              0
#define DOS

#endif  /* DJGPP_DOS */

/***************************************************************************/

#ifdef UNIX

#include <sys/param.h>

#define OPSYS_SEP             "/"
#define DIR_EXT             "dr9"
#define DIR83_EXT           "dir"
#define DATA_EXT            "dat"
#define NIT_EXT             "nit"

#define PATH_DELIMITER1       ':'
#define PATH_DELIMITER2       '!'

#define PATH_SIZE      MAXPATHLEN         
#define MacroPROMOTE            1
#define UseSTRICMP              1

#endif  /* UNIX */

/***************************************************************************/

#ifdef _POSIX_SOURCE

#include <limits.h>

#define OPSYS_SEP             "/"
#define DIR_EXT             "dr9"
#define DIR83_EXT           "dir"
#define DATA_EXT            "dat"
#define NIT_EXT             "nit"

#define PATH_DELIMITER1       ':'
#define PATH_DELIMITER2       '!'

#ifndef PATH_SIZE
#define PATH_SIZE _POSIX_PATH_MAX
#endif

#define MacroPROMOTE            1
#define UseSTRICMP              1

#endif  /* _POSIX_SOURCE */

/***************************************************************************/

#ifdef ULTRIX

#define INFO_SWAP

#include <sys/param.h>

#define OPSYS_SEP             "/"
#define DIR_EXT             "dr9"
#define DIR83_EXT           "dir"
#define DATA_EXT            "dat"
#define NIT_EXT             "nit"

#define PATH_DELIMITER1       ':'
#define PATH_DELIMITER2       '!'

#ifndef PATH_SIZE
#define PATH_SIZE      MAXPATHLEN
#endif

#define MacroPROMOTE            1
#define UseSTRICMP              1

#endif  /* ULTRIX */

/***************************************************************************/

#define INFO_FAIL               0
#define INFO_SUCCESS            1

#define DEFAULT_USER        "ARC"
#define DEFAULT_PATH       "info"

#define ITEMNAME_SIZE          17
#define INTERNALNAME_SIZE      33
#define USERNAME_SIZE           9
#define EXTNAME_SIZE            9

#define TMPRECORD_SIZE       4096
#define TMPSTRING_SIZE        128

#define DirEMPTY                1
#define DirNotEMPTY             0

#define DirDataFileTYPE         0
#define DirProgramTYPE          1
#define DirReportTYPE           2
#define DirSpecialFormTYPE      3
#define DirInputFormTYPE        4

#define INFO_MAX_DIRSIZE     9999

/***************************************************************************/

typedef struct
{
  char          InternalName[32];      /* user file name */
  char          ExternalName[8];       /* INFO file name */
  short int     NumberItems;           /* number of items */
  short int     RecordLength;          /* logical record length */
  char          Level0Password[4];     /* level 0 password */
  char          Level1Password[4];     /* level 1 password */
  char          Level2Password[4];     /* level 2 password */
  char          Level3Password[4];     /* level 3 password */
  short int     PrintWidth;            /* report output width */
  short int     EmptyFlag;             /* directory record empty flag */
#if BORLAND_C
  long int      NumberRecords;         /* number of records */
#else
  int           NumberRecords;         /* number of records */
#endif
  short int     Filler1;               /* not used */
  short int     FileType;              /* directory entry type */
  short int     Filler2;               /* ? */
  char          Filler3[4];            /* ? */
  char          ExternalFlag[2];       /* external data file flag */
  char          Filler4[300];          /* ? and not used */
} DirRecord;

typedef struct
{
  char          ItemName[16];          /* item name */
  short int     ItemWidth;             /* internal item width */
  short int     KeyType;               /* key type */
  short int     ItemPosition;          /* item position */
  short int     ProtectionLevel;       /* item protection level */
  short int     Filler1;               /* ? */
  short int     OutputWidth;           /* item output width */
  short int     NumberDecimals;        /* number of decimal places */
  short int     ItemType;              /* item type */
  short int     ReadOnlyFlag;          /* read only flag */
  short int     KeyLevel;              /* key level */
  short int     IndexNumber;           /* item index number */
  short int     OccuranceCount;        /* number of occurrences */
  short int     Filler2;               /* ? */
  char          AlternateName[16];     /* item alternate name */
  char          Filler3[16];           /* ? */
  short int     ConcordLevel;          /* concordance level */
  short int     AlternateRule;         /* item naming rule */
  char          Filler4[32];           /* ? */
  short int     ConcordCase;           /* concordance case */
  short int     ConcordNumbers;        /* concordance numbers */
  short int     ItemNumber;            /* logical item number */
  char          Filler5[28];           /* not used */
} NitRecord;

/***************************************************************************/

enum dirtype_t {Old = 1, New = 2};
typedef enum dirtype_t dirtype_t;

/*
 * These macros rely on the global "dirtype".  This global is defined in
 * infofile.c, and (so far, anyways) declared extern in infoitem.c.  Any
 * other files that need it will have to declare it as well.  Take note:
 * these macros don't end with a ";"
 *
 * dirtype stores the type (old-style, or 8.3) of the info directory.
 * Directory typing was introduced in Arc/Info 7.0 so that an info *
 * directory can contain more than 999 files (9999, actually) and so
 * that * the file names in an info directory comply with the 8.3 file
 * naming * convention on PC's.
 *
 * I chose to set the value of dirtyp whenever a path to the dr9 (or,
 * .dat!) file is generated.  All code that opens the dr9 file first
 * generates an old-style name, and tests the file's existence.  If the
 * file exists, it continues processing, and dirtype is set to "Old".
 * If the test fails, the code generates an 8.3 file name, and dirtype
 * is set to "New".  If the file is found in either case, subsequent
 * attempts to generate dat and nit file names will use the value of
 * dirtype.  If the dr9 file is not found, dirtype is still left as New,
 * though this shouldn't bother subsequent, since all infotools programs
 * bomb if the info directory cannot be opened. */


#define MAKEDIROLD(dir, info, user)				\
  dirtype = Old;						\
  sprintf (dir, "%s%s%s%s", info, OPSYS_SEP, user, DIR_EXT);	\
  StringtoLower (dir)


#define MAKEDIR83(dir, info, user)				\
  dirtype = New;						\
  sprintf (dir, "%s%s%s.%s", info, OPSYS_SEP, user, DIR83_EXT);	\
  StringtoLower (dir)


#define MAKEDAT(path, infodir, extname)					\
  switch (dirtype) {							\
  case Old:								\
      sprintf (path,"%s%s%s%s",infodir,OPSYS_SEP,extname,DATA_EXT);	\
      break;								\
  case New:								\
      sprintf (path,"%s%s%s.%s",infodir,OPSYS_SEP,extname,DATA_EXT);	\
      break;								\
  default:								\
      path[0] = 0;							\
  }									\
  StringtoLower (path)


#define MAKENIT(path, infodir, extname)					\
  switch (dirtype) {							\
  case Old:								\
      sprintf (path,"%s%s%s%s",infodir,OPSYS_SEP,extname,NIT_EXT);	\
      break;								\
  case New:								\
      sprintf (path,"%s%s%s.%s",infodir,OPSYS_SEP,extname,NIT_EXT);	\
      break;								\
  default:								\
      path[0] = 0;							\
  }									\
  StringtoLower (path);

#endif  /* _INFODEFS_H_ */
