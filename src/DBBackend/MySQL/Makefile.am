## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES = -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../.. $(all_includes)


# Instructions for building the convenience library
noinst_LTLIBRARIES=libkrecmysql.la
libkrecmysql_la_SOURCES=mysqlrecipedb.cpp
libkrecmysql_la_METASOURCES=AUTO


#the library search path.
libkrecmysql_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
