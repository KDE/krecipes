## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES = -I$(srcdir) -I$(srcdir)/.. $(all_includes)

# Check for optional database libs


if link_lib_SQLITE
sqlite_subdirs=libqsqlite SQLite
sqlite_libs=SQLite/krecsqlite.la
endif

if link_lib_MYSQL
mysql_subdirs=MySQL
mysql_libs=MySQL/krecmysql.la
endif

# Optional subdirectories

SUBDIRS=$(mysql_subdirs) $(sqlite_subdirs)

# Instructions for building the convenience library
noinst_LTLIBRARIES=libkrecipesdbs.la
libkrecipesdbs_la_SOURCES=recipedb.cpp 
libkrecipesdbs_la_METASOURCES=AUTO

#the library search path.
libkrecipesdbs_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) $(mysql_libs) $(sqlite_libs)
