## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes) -I.. -I../DBBackend

noinst_LTLIBRARIES=libkrecipesimporters.la
libkrecipesimporters_la_SOURCES = mx2importer.cpp mmfimporter.cpp mxpimporter.cpp nycgenericimporter.cpp baseimporter.cpp
libkrecipesimporters_la_METASOURCES=AUTO

#the library search path.
libkrecipesimporters_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
noinst_HEADERS =  kremanager.h
lib_LTLIBRARIES =  libkremanager.la
libkremanager_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
libkremanager_la_SOURCES =  kremanager.cpp
