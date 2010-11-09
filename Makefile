# Makefile
# --------------------------------------------------------------
# This will create executable files of the C programs that are
# part of my dbf utility package.  The libraries must be made first.
#
# Normally this is all done by dbfinstall.
#
# To invoke: make all
#
# Randy Deardorff.
#
# Changes:
#
# 11/09/94 -- Refer to math library as $MATHLIB, defaulting to
# /usr/lib/libm.a.  Prior to this, it was "-l m".  This proved to be a
# problem on some systems.  See notes below if this is a problem on your
# system.  Split the line with backslash since it was too long.
#
# 01/04/94 -- Added infodel.
#
# 12/16/96 -- Made CC=gcc and added CFLAGS=-traditional, and use -lm
# rather than /usr/lib/libm.a.  These seem to be required at 4.11T.
# Also had to add to the makefiles for each library, and my compile
# scripts (for things without make files).
#
# --------------------------------------------------------------
#
# If you use gnu c, you might want to leave these things as is.
# If that doesn't work, try setting CFLAGS to nothing, and 
# not setting CC at all.
#
CFLAGS=-traditional
#CFLAGS=
CC=gcc
#
# Set the path to the math library.  If yours is not as shown, change
# it.  You might be able to substitute "-lm".  This works on Data
# General DGUX, and Solaris 2.3.  But NOT on Sun or DEC-Alpha.
# Specifying the full path should work, as long as its correct.
# /usr/lib/libm.a is correct for DG, SGI, Sun, and DEC-Alpha, probably
# others too.
#
MATHLIB=	-lm
#MATHLIB=	/usr/lib/libm.a

LIBRARIES=	dbf/dbflib.a \
		infox/infoxlib.a \
		map/maplib.a \
		misc/misclib.a

EXEFILES=	info2dbf \
		dbf2info \
		info2ascii \
		ascii2info \
		makecoocoo \
		makegencoo \
		infolook \
		infoitems \
		infolist \
		infodir \
		infodel \
		dbflook \
		dbfitems \
		dbflist

all: $(EXEFILES)

${EXEFILES}:	$$@.c rbdpaths.h
		$(CC) $@.c $(LIBRARIES) $(MATHLIB) $(CFLAGS) -o $@
