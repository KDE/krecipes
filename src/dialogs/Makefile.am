## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES         = -I$(srcdir)/.. $(all_includes)

noinst_LTLIBRARIES=libkrecipesdialogs.la
libkrecipesdialogs_la_SOURCES=advancedsearchdialog.cpp recipeimportdialog.cpp dietwizarddialog.cpp recipeinputdialog.cpp recipeviewdialog.cpp selectrecipedialog.cpp ingredientsdialog.cpp selectunitdialog.cpp createelementdialog.cpp propertiesdialog.cpp createpropertydialog.cpp selectpropertydialog.cpp unitsdialog.cpp dependanciesdialog.cpp shoppinglistdialog.cpp shoppinglistviewdialog.cpp selectcategoriesdialog.cpp categorieseditordialog.cpp authorsdialog.cpp selectauthorsdialog.cpp dualprogressdialog.cpp resizerecipedialog.cpp dietviewdialog.cpp ingredientmatcherdialog.cpp usdadatadialog.cpp prepmethodsdialog.cpp createcategorydialog.cpp borderdialog.cpp refineshoppinglistdialog.cpp dbimportdialog.cpp createunitdialog.cpp
libkrecipesdialogs_la_METASOURCES=AUTO

#the library search path.
libkrecipesdialogs_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
