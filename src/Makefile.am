## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = krecipes

SUBDIRS = DBBackend importers gui dialogs exporters widgets

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
krecipes_LDADD = gui/libkstartuplogo.la DBBackend/libkrecipesdbs.la exporters/libkreexport.la importers/libkrecipesimporters.la dialogs/libkrecipesdialogs.la widgets/libkrecipeswidgets.la $(qsqlite_libadds) $(mysql_libadds) $(LIB_KHTML) $(LIB_KSPELL) $(DATABASE_LIBS)

# which sources should be compiled for krecipes
krecipes_SOURCES = main.cpp krecipes.cpp krecipesview.cpp pref.cpp krecipesiface.skel recipeinputdialog.cpp recipeviewdialog.cpp recipe.cpp ingredient.cpp ingredientlist.cpp elementlist.cpp element.cpp selectrecipedialog.cpp ingredientsdialog.cpp selectunitdialog.cpp createelementdialog.cpp propertiesdialog.cpp createpropertydialog.cpp ingredientproperty.cpp ingredientpropertylist.cpp selectpropertydialog.cpp editbox.cpp unitsdialog.cpp conversiontable.cpp unitratio.cpp unitratiolist.cpp propertycalculator.cpp setupwizard.cpp dependanciesdialog.cpp shoppinglistdialog.cpp shoppinglistviewdialog.cpp shoppingcalculator.cpp dietwizarddialog.cpp selectcategoriesdialog.cpp categorieseditordialog.cpp authorsdialog.cpp selectauthorsdialog.cpp fractioninput.cpp mixednumber.cpp menugroup.cpp dualprogressdialog.cpp resizerecipedialog.cpp serverprefs.ui unitsprefs.ui importprefs.ui

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/krecipes.pot

KDE_ICON = krecipes

# this is where the kdelnk file will go
kdelnkdir   = $(kde_appsdir)/Utilities
kdelnk_DATA = krecipes.desktop

# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/krecipes
rc_DATA = krecipesui.rc
