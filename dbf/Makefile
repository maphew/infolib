# Makefile
#------------------------------------------------------------------------
# This will create a library of my dbf related functions.
#
# You do not need to edit this file.  The install script edits it.
# If you edit it, the install script might fail.
#
# Randy Deardorff.
#------------------------------------------------------------------------
CFLAGS=-traditional
CC=gcc
#
# The list of object files.
#
objects=	dbfgethead.o \
		dbfgetdefs.o \
		dbfgetrec.o \
		dbfputhead.o \
		dbfputdefs.o \
		dbfputrec.o \
		dbfputtail.o \
		dbfgetdate.o \
		dbferror.o
#
# Create library, inserting all object files.
#
dbflib.a:	$(objects)
		ar cr dbflib.a $(objects)
#		ranlib dbflib.a
#
# Compile any out of date sources to object files.
#
$(objects):	dbf.h dbfsys.h ../rbdpaths.h
