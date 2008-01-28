
include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/../ )

add_library(psqlrecipedb SHARED psqlrecipedb.cpp)
target_link_libraries( psqlrecipedb )

########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

### Makefile.am for krecipes
#
## this is the program that gets installed.  it's name is used for all
## of the other Makefile.am variables
#
## set the include path for X, qt and KDE
#INCLUDES = -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../.. $(all_includes)
#
#
## Instructions for building the convenience library
#noinst_LTLIBRARIES=libkrecpsql.la
#libkrecpsql_la_SOURCES=psqlrecipedb.cpp
#libkrecpsql_la_METASOURCES=AUTO
#
#
##the library search path.
#libkrecpsql_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
