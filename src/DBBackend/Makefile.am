## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES = -I$(srcdir) -I$(srcdir)/.. $(all_includes)

# Check for optional database libs


if link_lib_SQLITE
sqlite_subdirs=libqsqlite SQLite
sqlite_libs=SQLite/libkrecsqlite.la
endif

if link_lib_MYSQL
mysql_subdirs=MySQL
mysql_libs=MySQL/libkrecmysql.la
endif

psql_libs=PostgreSQL/libkrecpsql.la
psql_subdirs=PostgreSQL

# Optional subdirectories

SUBDIRS=$(sqlite_subdirs) $(mysql_subdirs) $(psql_subdirs)

# Instructions for building the convenience library
noinst_LTLIBRARIES=libkrecipesdbs.la
libkrecipesdbs_la_SOURCES=recipedb.cpp qsqlrecipedb.cpp
libkrecipesdbs_la_METASOURCES=AUTO

libkrecipesdbs_la_LIBADD= $(mysql_libs) $(sqlite_libs) $(psql_libs)

#the library search path.
libkrecipesdbs_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
