/* rbdpaths.h
   ------------------------------------------------------------------------
   Sets system specific stuff, including paths to all my header files.
   Must be included by any programs that need to call any of my functions.
   ----------------------------------------------------------------------*/

/* Choose target os.
   
   This setting has two implications.  The first is simply whether
   we're on a UNIX box or a PC.  In all of my libraries, BORLAND_C
   means Borland C++ 3.1, DJGPP_DOS means djgpp, the DOS version of
   GNU C++ on an ibm pc.  If both BORLAND_C and DJGPP_DOS are zero, I
   assume you are on ANY_UNIX.

   Note that all the libraries work on the PC with either Borland or
   Djgpp.  Under Borland, the mains may blow if you process a large
   file.  This is not true for Djgpp, which is a 32 bit DOS extended
   compiler.
   
   The second implication has to do with the infoxlib.  For Todd
   Stellhorn's direct info access functions (which are built into
   infoxlib), we need to be a bit more specific about which UNIX
   system we are on.  If you are on a DEC/Alpha OSF system, choose
   DEC_OSF.  For everything else, choose DG_UX or SUN_OS.  It makes no
   difference.  The only one that is different is DEC_OSF.
   
   
   Here is a list of some machines and the setting that works on it.
   
   Data General AViiON DGUX                    DG_UX
   Sun SUNOS                                   DG_UX
   Sun Solaris                                 DG_UX
   Silicon Graphics indigo IRIX                DG_UX
   IBM RS6000                                  DG_UX
   DEC/Alpha OSF                               DEC_OSF
   IBM PC, Borland C++ 3.1                     BORLAND_C
   IBM PC, Djgpp 1.12 maint 4                  DJGPP_DOS

   */

#define DG_UX         1
#define SUN_OS        0
#define DEC_OSF       0
#define BORLAND_C     0
#define DJGPP_DOS     0


/* Don't change anything below, unless you have already tried the install
   script and it failed and you are trying to fix whatever went wrong.
   
   Everything below here should be set correctly by the configure phase
   of the install script.  */


/* Set whether or not to byte swap binary values coming out of or
   going into an MS-DOS binary file.  It is CRUCIAL that this be set
   correctly for your system.  HOWEVER, The install script will
   attempt to set this switch by testing whether swapping is required
   on your system.  You should thus not need to set it yourself.  */

#define DOS_SWAP 1


/* Set appropriate paths for platform.  You do NOT need to edit these.
   The install script edits them for you.  If, after running the
   install script, any paths are not correct, then they are not needed,
   so it's ok that they are incorrect. */

#if BORLAND_C | DJGPP_DOS
#define ANY_UNIX 0
#else
#define ANY_UNIX 1
#endif

#if ANY_UNIX
#define DOS        0
#define SYSSEP     47
#define MISC_H "/usr133/rbd/gis/util/dbf2info/misc/misc.h"
#define MAP_H "/usr133/rbd/gis/util/dbf2info/map/map.h"
#define INFO_H "/usr133/rbd/gis/util/dbf2info/infox/infox.h"
#define INFOLIB_H "/usr133/rbd/gis/util/dbf2info/infox/infolib.h"
#define INFODEFS_H "/usr133/rbd/gis/util/dbf2info/infox/infodefs.h"
#define INFOSWAP_H "/usr133/rbd/gis/util/dbf2info/infox/infoswap.h"
#define DBF_H "/usr133/rbd/gis/util/dbf2info/dbf/dbf.h"
#define DBFSYS_H "/usr133/rbd/gis/util/dbf2info/dbf/dbfsys.h"
#define E00_H "/usr133/rbd/gis/util/dbf2info/e00/e00.h"
#define GD_H       "/usr133/rbd/c/gd/gd.h"
#define MASTER_H   "/usr133/rbd/gis/sites/master.h"

/* Set target OS as required by Todd Stellhorn's INFO direct stuff.
   Don't change anything here. */

#undef  UNIX
#define UNIX
#if DEC_OSF
#undef UNIX
#undef ULTRIX
#define ULTRIX
#endif

#else

/* Not UNIX, therefore PC.
   If you want to compile this stuff on a PC, you will have to edit the
   paths below to reflect what they are on your system.
   
   */

#undef UNIX
#undef ULTRIX
#define DOS        1
#define SYSSEP     92
#define MISC_H "/usr133/rbd/gis/util/dbf2info/misc/misc.h"
#define MAP_H "/usr133/rbd/gis/util/dbf2info/map/map.h"
#define INFO_H "/usr133/rbd/gis/util/dbf2info/infox/infox.h"
#define INFOLIB_H "/usr133/rbd/gis/util/dbf2info/infox/infolib.h"
#define INFODEFS_H "/usr133/rbd/gis/util/dbf2info/infox/infodefs.h"
#define INFOSWAP_H "/usr133/rbd/gis/util/dbf2info/infox/infoswap.h"
#define DBF_H "/usr133/rbd/gis/util/dbf2info/dbf/dbf.h"
#define DBFSYS_H "/usr133/rbd/gis/util/dbf2info/dbf/dbfsys.h"
#define E00_H "/usr133/rbd/gis/util/dbf2info/e00/e00.h"
#define MASTER_H   "d:\c\gis\sites\master.h"
#endif


/* These should be set in stdio.h, but aren't on some systems. */

#ifndef SEEK_SET
#define SEEK_SET 0L
#endif
#ifndef SEEK_CUR
#define SEEK_CUR 1L
#endif
#ifndef SEEK_END
#define SEEK_END 2L
#endif
