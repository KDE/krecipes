## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = krecipes krecipes_client

# set the include path for X, qt and KDE
INCLUDES         = $(all_includes)

# the library search path.
krecipes_LDFLAGS = $(KDE_RPATH) $(all_libraries)

# the libraries to link against.
krecipes_LDADD = -lmysqlclient $(LIB_KFILE) $(LIB_KDEPRINT) $(LIB_KHTML)

# which sources should be compiled for krecipes
krecipes_SOURCES = main.cpp krecipes.cpp krecipesview.cpp 		      pref.cpp krecipesiface.skel recipedb.h recipedb.cpp recipeinputdialog.h recipeinputdialog.cpp recipeviewdialog.h recipeviewdialog.cpp recipe.h recipe.cpp ingredient.h ingredient.cpp ingredientlist.h ingredientlist.cpp elementlist.h elementlist.cpp element.h element.cpp selectrecipedialog.h selectrecipedialog.cpp ingredientsdialog.h ingredientsdialog.cpp selectunitdialog.h selectunitdialog.cpp createelementdialog.h createelementdialog.cpp image.h propertiesdialog.h propertiesdialog.cpp createpropertydialog.h createpropertydialog.cpp ingredientproperty.h ingredientproperty.cpp ingredientpropertylist.h ingredientpropertylist.cpp selectpropertydialog.h selectpropertydialog.cpp editbox.h editbox.cpp unitsdialog.h unitsdialog.cpp conversiontable.h conversiontable.cpp unitratio.h unitratio.cpp unitratiolist.h unitratiolist.cpp propertycalculator.cpp propertycalculator.h setupwizard.h setupwizard.cpp dependanciesdialog.h dependanciesdialog.cpp shoppinglistdialog.h shoppinglistdialog.cpp shoppinglistviewdialog.h shoppinglistviewdialog.cpp shoppingcalculator.h shoppingcalculator.cpp dietwizarddialog.h dietwizarddialog.cpp selectcategoriesdialog.h selectcategoriesdialog.cpp categorieseditordialog.cpp authorsdialog.cpp selectauthorsdialog.cpp fractioninput.h fractioninput.cpp fraction.h fraction.cpp menugroup.h menugroup.cpp mxpimporter.h mxpimporter.cpp mmfimporter.h mmfimporter.cpp baseimporter.h baseimporter.cpp

# these are the headers for your project
noinst_HEADERS = krecipes.h krecipesview.h pref.h categorieseditordialog.h authorsdialog.h selectauthorsdialog.h

# client stuff
krecipes_client_LDFLAGS = $(KDE_RPATH) $(all_libraries)
krecipes_client_LDADD   = $(LIB_KDECORE)
krecipes_client_SOURCES = krecipes_client.cpp

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
