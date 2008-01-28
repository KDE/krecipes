
include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/../ )

add_subdirectory( libqsqlite )

set(literecipedb_SRCS
   literecipedb.cpp
   qsql_sqlite.cpp)

kde4_add_library(literecipedb SHARED ${literecipedb_SRCS})
target_link_libraries( literecipedb )




########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

### Makefile.am for krecipes
#
## this is the program that gets installed.  it's name is used for all
## of the other Makefile.am variables
#
## set the include path for X, qt and KDE
#INCLUDES         = $(all_includes)
#
##building the library
#
#noinst_LTLIBRARIES=libkrecqsqlite.la
#libkrecqsqlite_la_SOURCES=krecqsqlitedb.cpp krecqsqliteresult.cpp
#libkrecqsqlite_la_METASOURCES=AUTO
#
##the library search path.
#libkrecqsqlite_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#
