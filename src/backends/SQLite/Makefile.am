## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES = -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../.. $(all_includes)

SUBDIRS = libqsqlite

# Instructions for building the convenience library
noinst_LTLIBRARIES=libkrecsqlite.la
libkrecsqlite_la_SOURCES=literecipedb.cpp
libkrecsqlite_la_METASOURCES=AUTO

libkrecsqlite_la_LIBADD=libqsqlite/libkrecqsqlite.la

#the library search path.
libkrecsqlite_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
