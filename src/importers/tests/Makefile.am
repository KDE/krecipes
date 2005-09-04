INCLUDES = -I$(srcdir)/../.. -I$(srcdir)/../../importers $(all_includes)

AM_LDFLAGS = $(KDE_RPATH) $(all_libraries)

check_PROGRAMS = mmftest

noinst_HEADERS = importertest.h

METASOURCES = AUTO

LDADD = ../libkrecipesimporters.la ../../datablocks/libdatablocks.la $(LIB_QT) $(LIB_KHTML) $(LIB_KSPELL)
mmftest_SOURCES = mmftest.cpp
