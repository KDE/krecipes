## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES         = -I.. $(all_includes)

noinst_LTLIBRARIES=libkrecipeswidgets.la
libkrecipeswidgets_la_SOURCES=sizehandle.cpp dragarea.cpp krelistview.cpp kremenu.cpp
libkrecipeswidgets_la_METASOURCES=AUTO

#the library search path.
libkrecipeswidgets_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
