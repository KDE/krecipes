## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = krecipes

SUBDIRS = DBBackend importers gui dialogs

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
krecipes_LDADD = gui/libkstartuplogo.la DBBackend/libkrecipesdbs.la importers/libkrecipesimporters.la dialogs/libkrecipesdialogs.la $(qsqlite_libadds) $(mysql_libadds) $(LIB_KHTML) $(LIB_KSPELL) $(DATABASE_LIBS)

# which sources should be compiled for krecipes
krecipes_SOURCES = main.cpp krecipes.cpp krecipesview.cpp pref.cpp krecipesiface.skel recipeinputdialog.h recipeinputdialog.cpp recipeviewdialog.h recipeviewdialog.cpp recipe.h recipe.cpp ingredient.h ingredient.cpp ingredientlist.h ingredientlist.cpp elementlist.h elementlist.cpp element.h element.cpp selectrecipedialog.h selectrecipedialog.cpp ingredientsdialog.h ingredientsdialog.cpp selectunitdialog.h selectunitdialog.cpp createelementdialog.h createelementdialog.cpp image.h propertiesdialog.h propertiesdialog.cpp createpropertydialog.h createpropertydialog.cpp ingredientproperty.h ingredientproperty.cpp ingredientpropertylist.h ingredientpropertylist.cpp selectpropertydialog.h selectpropertydialog.cpp editbox.h editbox.cpp unitsdialog.h unitsdialog.cpp conversiontable.h conversiontable.cpp unitratio.h unitratio.cpp unitratiolist.h unitratiolist.cpp propertycalculator.cpp propertycalculator.h setupwizard.h setupwizard.cpp dependanciesdialog.h dependanciesdialog.cpp shoppinglistdialog.h shoppinglistdialog.cpp shoppinglistviewdialog.h shoppinglistviewdialog.cpp shoppingcalculator.h shoppingcalculator.cpp dietwizarddialog.h dietwizarddialog.cpp selectcategoriesdialog.h selectcategoriesdialog.cpp categorieseditordialog.cpp authorsdialog.cpp selectauthorsdialog.cpp fractioninput.h fractioninput.cpp mixednumber.h mixednumber.cpp menugroup.h menugroup.cpp dualprogressdialog.h dualprogressdialog.cpp resizerecipedialog.cpp

# these are the headers for your project
noinst_HEADERS = krecipes.h krecipesview.h pref.h categorieseditordialog.h authorsdialog.h selectauthorsdialog.h

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
