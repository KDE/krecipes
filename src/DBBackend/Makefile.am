## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# Check for optional database libs
if link_lib_MYSQL
mysql_sources=mysqlrecipedb.cpp
endif

if link_lib_SQLITE
qsqlite_subdirs=libqsqlite
sqlite_sources=literecipedb.cpp
endif

# Optional subdirectories

SUBDIRS=$(qsqlite_subdirs)

# Instructions for building the convenience library
noinst_LTLIBRARIES=libkrecipesdbs.la
libkrecipesdbs_la_SOURCES=recipedb.cpp $(sqlite_sources) $(mysql_sources)
libkrecipesdbs_la_METASOURCES=AUTO

#the library search path.
libkrecipesdbs_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)