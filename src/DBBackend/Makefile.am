## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

SUBDIRS=libqsqlite

noinst_LTLIBRARIES=libkrecipesdbs.la
libkrecipesdbs_la_SOURCES=recipedb.cpp recipedb.h literecipedb.cpp literecipedb.h mysqlrecipedb.cpp mysqlrecipedb.h
libkrecipesdbs_la_METASOURCES=AUTO

#the library search path.
libkrecipesdbs_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)