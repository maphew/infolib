/* misc.h
   ------------------------------------------------------------------------
   This header declares the functions kept in misclib.a.
   
   You must include rbdpaths.h first to set DG_UX, or whatever.
   
   Written: 06/26/92 Randy Deardorff.  USEPA Seattle.
   
   Modification log: (all changes are by the author unless otherwise noted)
   
   07/30/94 Changed getcwd to syscwd for compatibility with BORLAND_C
   12/16/94 Added sysstat.
   01/14/95 Changed PC exclusions (eg strupr) to use if ANY_UNIX.
   02/03/95 Minor changes for DJGPP_DOS.
   ------------------------------------------------------------------------ */

/* Function prototypes. */

char*    ctoa();
char*    input();
char     inputc();
double   inputd();
float    inputf();
int      inputi();
long     inputl();
int      usage();
int      strafter();
int      strbefore();
int      strdel();
char*    strdir();
int      stredt();
char*    strentry();
int      strext();
int      strqext();
int      strfind();
int      stricom();
int      strinsert();
int      strisnum();
int      stritems();
int      strqitems();
int      strappend();
int      strjus();
int      strkeyword();
int      strlakw();
int      strlist();
int      strlshift();
int      strcoll();
int      strnull();
int      stroverlay();
int      strmatch();
int      strpad();
int      strput();
int      strquote();
int      strqquote();
int      strrfind();
int      strsubstr();
int      strtrim();
int      strtype();
int      strunquote();
int      strqunquote();
int      sysdate();
int      sysrename();
int      sysdel();
int      sysexist();
int      sysstat();
char*    syscwd();
int      yesno();

#if ANY_UNIX
char*    getuser();
char*    inputpwd();
int      strlwr();
int      strupr();
#endif
