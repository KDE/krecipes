## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

#building the library

noinst_LTLIBRARIES=libkrecqsqlite.la
libkrecqsqlite_la_SOURCES=krecqsqlitedb.cpp krecqsqliteresult.cpp
libkrecqsqlite_la_METASOURCES=AUTO

#the library search path.
libkrecqsqlite_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)

