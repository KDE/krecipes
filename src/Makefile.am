## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = krecipes

SUBDIRS = DBBackend importers gui dialogs exporters widgets datablocks

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# the library search path.
krecipes_LDFLAGS = $(KDE_RPATH) $(all_libraries)


# Check for optional libs
if link_lib_SQLITE
qsqlite_libadds =DBBackend/libqsqlite/libkrecqsqlite.la -lsqlite
endif

if link_lib_MYSQL
mysql_libadds =-lmysqlclient
endif

# the shared libraries to link against.
krecipes_LDADD = gui/libkstartuplogo.la DBBackend/libkrecipesdbs.la exporters/libkreexport.la importers/libkrecipesimporters.la dialogs/libkrecipesdialogs.la widgets/libkrecipeswidgets.la datablocks/libdatablocks.la $(qsqlite_libadds) $(mysql_libadds) $(LIB_KHTML) $(LIB_KSPELL) $(DATABASE_LIBS)

# which sources should be compiled for krecipes
krecipes_SOURCES = main.cpp krecipes.cpp krecipesview.cpp pref.cpp krecipesiface.skel recipe.cpp ingredient.cpp ingredientlist.cpp elementlist.cpp element.cpp ingredientproperty.cpp ingredientpropertylist.cpp editbox.cpp conversiontable.cpp unitratio.cpp unitratiolist.cpp propertycalculator.cpp setupwizard.cpp shoppingcalculator.cpp fractioninput.cpp mixednumber.cpp

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

KDE_ICON = krecipes

# this is where the kdelnk file will go
kdelnkdir   = $(kde_appsdir)/Utilities
kdelnk_DATA = krecipes.desktop

# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/krecipes
rc_DATA = krecipesui.rc
