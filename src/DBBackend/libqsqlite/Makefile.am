## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

#building the library

lib_LTLIBRARIES=libkrecqsqlite.la
libkrecqsqlite_la_SOURCES=krecqsqlitedb.cpp  krecqsqlitedb.h  krecqsqliteresult.cpp  krecqsqliteresult.h
libkrecqsqlite_la_METASOURCES=AUTO

#the library search path.
libkrecqsqlite_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)

#install the following headers
include_HEADERS = krecqsqliteresult.h krecqsqlitedb.h