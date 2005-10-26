INCLUDES = -I$(srcdir) -I$(srcdir)/.. $(all_includes)

METASOURCES = AUTO
noinst_LTLIBRARIES =  libkrecipesexporters.la

libkrecipesexporters_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)

libkrecipesexporters_la_SOURCES =  kreexport.cpp baseexporter.cpp cookmlexporter.cpp \
	recipemlexporter.cpp mmfexporter.cpp htmlexporter.cpp plaintextexporter.cpp \
	rezkonvexporter.cpp

noinst_HEADERS =  kreexport.h baseexporter.h cookmlexporter.h \
	recipemlexporter.h mmfexporter.h htmlexporter.h plaintextexporter.h
