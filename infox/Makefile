# Makefile
#------------------------------------------------------------------------
# This will create a library of my info file related functions.
# Some of these functions are by Todd Stellhorn of ESRI.
#
# You do not need to edit this file.  The install script edits it.
# If you edit it, the install script might fail.
#
# Randy Deardorff.
#
# 12/14/94 Added dependency on all headers rather than just infox.h
#------------------------------------------------------------------------
CFLAGS=-traditional
CC=gcc
#
# The list of object files.
#
objects=	ascgetdefs.o \
		ascputdefs.o \
		headinfo2dbf.o \
		defsinfo2dbf.o \
		recinfo2dbf.o \
		defsdbf2info.o \
		recdbf2info.o \
		infoerror.o \
		infoderror.o \
		recinfo2asc.o \
		recasc2info.o \
		getnumredef.o \
		getitems.o \
		putitems.o \
		infodir.o \
		infoetc.o \
		infofile.o \
		infoitem.o \
		infoname.o \
		infonit.o \
		inforec.o \
		infostr.o \
		infoswap.o \
		infoutil.o

#
# Create library, inserting all object files.
#
infoxlib.a:	$(objects)
		ar cr infoxlib.a $(objects)
#		ranlib infoxlib.a
#
# Compile any out of date sources to object files.
#
$(objects):	infox.h infodefs.h infolib.h infoswap.h ../rbdpaths.h
