
include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/../ )

set(krecipesimporters_SRCS
   mx2importer.cpp
   mmfimporter.cpp
   mxpimporter.cpp
   nycgenericimporter.cpp
   recipemlimporter.cpp
   baseimporter.cpp
   kreimporter.cpp
   rezkonvimporter.cpp
   kredbimporter.cpp)

add_library(krecipesimporters SHARED ${krecipesimporters_SRCS})


########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

### Makefile.am for krecipes
#
## this is the program that gets installed.  it's name is used for all
## of the other Makefile.am variables
#
## set the include path for X, qt and KDE
#INCLUDES         = -I$(srcdir)/.. -I$(srcdir)/../backends $(all_includes)
#
#noinst_LTLIBRARIES=libkrecipesimporters.la
#libkrecipesimporters_la_SOURCES = mx2importer.cpp mmfimporter.cpp mxpimporter.cpp nycgenericimporter.cpp recipemlimporter.cpp baseimporter.cpp kreimporter.cpp rezkonvimporter.cpp kredbimporter.cpp
#libkrecipesimporters_la_METASOURCES=AUTO
#
##the library search path.
#libkrecipesimporters_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#noinst_HEADERS = kreimporter.h
