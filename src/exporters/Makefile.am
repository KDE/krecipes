INCLUDES = -I $(srcdir)/.. -I$(srcdir)/../DBBackend $(all_includes)
METASOURCES = AUTO
lib_LTLIBRARIES =  libkrecipesexporters.la
libkrecipesexporters_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
libkrecipesexporters_la_SOURCES =  kreexport.cpp baseexporter.cpp cookmlexporter.cpp recipemlexporter.cpp mmfexporter.cpp htmlexporter.cpp
noinst_HEADERS =  kreexport.h baseexporter.h cookmlexporter.cpp recipemlexporter.h mmfexporter.h htmlexporter.h
