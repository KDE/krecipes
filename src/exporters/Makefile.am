INCLUDES = -I$(srcdir) -I$(srcdir)/.. $(all_includes)

METASOURCES = AUTO
noinst_LTLIBRARIES =  libkrecipesexporters.la

libkrecipesexporters_la_CXXFLAGS = $(LIBXML2_CFLAGS)
libkrecipesexporters_la_LDFLAGS = $(KDE_RPATH) $(all_libraries) $(LIBXML2_LIBS) -lxslt

libkrecipesexporters_la_SOURCES =  kreexporter.cpp baseexporter.cpp cookmlexporter.cpp \
	recipemlexporter.cpp mmfexporter.cpp htmlexporter.cpp plaintextexporter.cpp \
	rezkonvexporter.cpp htmlbookexporter.cpp xsltexporter.cpp

noinst_HEADERS =  kreexporter.h baseexporter.h cookmlexporter.h \
	recipemlexporter.h mmfexporter.h htmlexporter.h plaintextexporter.h \
	xsltexporter.h
