## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

lib_LTLIBRARIES=libkrecqsqlite.la
libkrecqsqlite_la_SOURCES=qsqlitedb.cpp  qsqlitedb.h  qsqliteresult.cpp  qsqliteresult.h
libkrecqsqlite_la_METASOURCES=AUTO

#the library search path.
libkrecqsqlite_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)