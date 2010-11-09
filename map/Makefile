# Makefile
#------------------------------------------------------------------------
# This will create a library of my map related functions.
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
objects=	alb2dd.o \
		dd2alb.o \
		dd2dms.o \
		dms2dd.o \
		dmsunpack.o \
		dmsstr2dbl.o \
		dmsdbl2str.o \
		dmsdbl2int.o \
		dmsint2dbl.o
#
# Create library, inserting all object files.
#
maplib.a:	$(objects)
		ar cr $@ $(objects)
#		ranlib $@
#
# Compile any out of date sources to object files.
#
$(objects):      map.h ../rbdpaths.h
