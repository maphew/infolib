# Makefile
#------------------------------------------------------------------------
# This will create a library of my miscellaneous C functions.
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
objects=	inputpwd.o \
		inputd.o \
		inputf.o \
		inputi.o \
		inputl.o \
		inputc.o \
		sysdate.o \
		sysrename.o \
		sysdel.o \
		usage.o \
		strjus.o \
		strkeyword.o \
		strlakw.o \
		strquote.o \
		strqquote.o \
		strunquote.o \
		strqunquote.o \
		strlist.o \
		strafter.o \
		strbefore.o \
		strisnum.o \
		strtrim.o \
		strdel.o \
		strdir.o \
		strentry.o \
		strput.o \
		strpad.o \
		stroverlay.o \
		strmatch.o \
		strext.o \
		strqext.o \
		stritems.o \
		strqitems.o \
		strappend.o \
		stredt.o \
		strfind.o \
		strrfind.o \
		strlwr.o \
		strupr.o \
		strtype.o \
		strsubstr.o \
		strnull.o \
		strinsert.o \
		strlshift.o \
		strcoll.o \
		stricom.o \
		sysexist.o \
		sysstat.o \
		input.o \
		yesno.o \
		syscwd.o \
		getuser.o \
		ctoa.o
#
# Create library, inserting all object files.
#
misclib.a:	$(objects)
		ar cr misclib.a $(objects)
#		ranlib misclib.a
#
# Compile any out of date sources to object files.
#
$(objects):      misc.h ../rbdpaths.h
