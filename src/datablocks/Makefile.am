## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES         = -I$(srcdir)/.. $(all_includes)

noinst_LTLIBRARIES=libdatablocks.la
libdatablocks_la_SOURCES=recipelist.cpp constraintlist.cpp categorytree.cpp
libdatablocks_la_METASOURCES=AUTO

#the library search path.
libdatablocks_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
