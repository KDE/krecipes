## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes) -I$(srcdir)/..

noinst_LTLIBRARIES=libkrecipesdialogs.la
libkrecipesdialogs_la_SOURCES=recipeimportdialog.cpp
libkrecipesdialogs_la_METASOURCES=AUTO

#the library search path.
libkrecipesdialogs_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
