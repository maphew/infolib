/* @(#)infolib.h	1.10 2/16/94 */
/*
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

/***************************************************************************
  Name: infolib.h
  
  Description: infolib main header.
  
  History: 12/91  Todd Stellhorn  - original coding
  
  **************************************************************************/

#ifndef _INFOLIB_H_
#define _INFOLIB_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef struct
{
  char         *FileName;          /* user file name */
  char         *DirFilePath;       /* path to DR9 file */
  char         *NitFilePath;       /* path to FileName NIT file */
  FILE         *InfoFILE;          /* file pointer */
  short int     NumberItems;       /* number of (real) items */
  short int     RecordLength;      /* logical record length */
  long int      NumberRecords;     /* number of records */
  short int     AccessFlag;        /* access flag */
  long int      CurrentRecord;     /* current record number */
  char         *IOBuffer;          /* input/output buffer */
} InfoFile;

typedef struct
{
  short int     ItemPosition;      /* item position */
  short int     ItemWidth;         /* item width */
  short int     ItemType;          /* item type */
  short int     NumberDecimals;    /* number of decimal places */
  short int     OutputWidth;       /* item output width */
  char          ItemName[17];      /* item name */
} InfoItemDef;

typedef InfoItemDef TemplateList;

/***************************************************************************/

#define InfoREAD                1      /* file access flags */
#define InfoWRITE               2
#define InfoINIT                3

#define INFO_DATE_TYPE          1      /* INFO item types */
#define INFO_CHARACTER_TYPE     2
#define INFO_INTEGER_TYPE       3
#define INFO_NUMBER_TYPE        4
#define INFO_BINARY_TYPE        5
#define INFO_FLOATING_TYPE      6
#define INFO_LEADFILL_TYPE      7
#define INFO_PACKED_TYPE        8
#define INFO_ZEROFILL_TYPE      9
#define INFO_OVERPUNCH_TYPE    10
#define INFO_TRAILINGSIGN_TYPE 11
#define INFO_RECNO_TYPE        99

#define INFO_RECNO       "$RECNO"

#define INFO_COVER_PAT          1      /* coverage file types */
#define INFO_COVER_AAT          2
#define INFO_COVER_NAT          3
#define INFO_COVER_BND          4
#define INFO_COVER_TIC          5
#define INFO_COVER_ACODE        6
#define INFO_COVER_ADD          7
#define INFO_COVER_TRN          8

#define CASE_SENSITIVE          1
#define NONCASE_SENSITIVE       2

/***************************************************************************/

/* infolib functions */

/* user level functions */

/* infofile */

InfoFile     *InfoOpenFile();
short int     InfoCloseFile();
short int     InfoCreateFile();
short int     InfoDeleteFile();
short int     InfoFileExists();
short int     InfoFileFlush();
long int      InfoNumberRecords();
short int     InfoNumberItems();
long int      InfoRecordLength();
short int     InfoIsOpenForWrite();
long int      InfoCurrentRecord();

/* inforec.c */

short int     InfoReadRecord();
short int     InfoReadNextRecord();
short int     InfoWriteRecord();
short int     InfoWriteNextRecord();

/* infoitem */

InfoItemDef  *InfoGetItemDef();
void          InfoItemDefFree();
short int     InfoDefineItem();
short int     InfoDecode();
short int     InfoEncode();
short int     InfoItemExists();
short int     InfoItemPosition();
short int     InfoItemWidth();
short int     InfoItemType();
short int     InfoItemNumberDecimals();
short int     InfoItemOutputWidth();

/* infoname */

short int     InfoParsePath();
short int     InfoCoverFileName();
short int     InfoTempName();

/* infoutil */

short int     InfoBinarySearch();
short int     InfoSeqSearch();
TemplateList *InfoGetTemplate();
short int     InfoFreeTemplate();

/* support functions */

/* infodir */

short int     DirGetRecord();
short int     DirReadRecord();
short int     DirWriteRecord();

/* infoetc */

FILE         *OpenFile();
short int     CloseFile();
short int     CompareValues();
short int     GetExternalPath();
char         *IOBufferAllocate();
void          InfoFileFree();

/* infonit */

short int     NitGetRecord();
short int     NitReadRecord();
short int     NitWriteRecord();

/* infostr */

void          TerminateString();
void          StringtoLower();
void          StringtoUpper();
void          FillStringBuffer();
void          CompressString();
void          ExpandString();
short int     CompareStrings();
char         *FetchNextWord();
char         *AMLQuote();

/* infoswap */

void          InfoByteSwap();

/***************************************************************************/

#endif  /* _INFOLIB_H_ */
