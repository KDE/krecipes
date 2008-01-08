INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../importers -I$(srcdir)/../exporters $(all_includes)

AM_LDFLAGS = $(KDE_RPATH) $(all_libraries)

check_PROGRAMS = kretest mmftest mx2test mxptest rezkonvtest nyctest recipemltest

noinst_HEADERS = importertest.h exportertest.h

METASOURCES = AUTO

LDADD = ../importers/libkrecipesimporters.la ../exporters/libkrecipesexporters.la ../backends/libkrecipesdbs.la ../datablocks/libdatablocks.la $(LIB_QT) $(LIB_KHTML) $(LIB_KSPELL)

kretest_SOURCES = kretest.cpp
mmftest_SOURCES = mmftest.cpp
mx2test_SOURCES = mx2test.cpp
mxptest_SOURCES = mxptest.cpp
rezkonvtest_SOURCES = rezkonvtest.cpp
nyctest_SOURCES = nyctest.cpp
recipemltest_SOURCES = recipemltest.cpp
