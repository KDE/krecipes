
include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

set(krecipesdialogs_SRCS
   advancedsearchdialog.cpp
   recipeimportdialog.cpp
   dietwizarddialog.cpp
   recipeinputdialog.cpp
   recipeviewdialog.cpp
   selectrecipedialog.cpp
   ingredientsdialog.cpp
   selectunitdialog.cpp
   createelementdialog.cpp
   propertiesdialog.cpp
   createpropertydialog.cpp
   selectpropertydialog.cpp
   unitsdialog.cpp
   dependanciesdialog.cpp
   shoppinglistdialog.cpp
   shoppinglistviewdialog.cpp
   selectcategoriesdialog.cpp
   categorieseditordialog.cpp
   authorsdialog.cpp
   selectauthorsdialog.cpp
   resizerecipedialog.cpp
   dietviewdialog.cpp
   ingredientmatcherdialog.cpp
   prepmethodsdialog.cpp
   createcategorydialog.cpp
   borderdialog.cpp
   refineshoppinglistdialog.cpp
   pagesetupdialog.cpp
   dbimportdialog.cpp
   createunitdialog.cpp
   setupdisplay.cpp
   ingredientparserdialog.cpp
   ingredientgroupsdialog.cpp
   editratingdialog.cpp
   similarcategoriesdialog.cpp
   conversiondialog.cpp
   createingredientweightdialog.cpp
   recipeprintpreview.cpp
   editpropertiesdialog.cpp
   createunitconversiondialog.cpp)

add_library(krecipesdialogs SHARED ${krecipesdialogs_SRCS})

########### next target ###############


########### install files ###############




#original Makefile.am contents follow:

### Makefile.am for krecipes
#
## this is the program that gets installed.  it's name is used for all
## of the other Makefile.am variables
#
## set the include path for X, qt and KDE
#INCLUDES         = -I$(srcdir)/.. $(all_includes)
#
#KDE_CXXFLAGS = $(USE_EXCEPTIONS)
#
#noinst_LTLIBRARIES=libkrecipesdialogs.la
#libkrecipesdialogs_la_SOURCES= \
#	advancedsearchdialog.cpp recipeimportdialog.cpp \
#	dietwizarddialog.cpp recipeinputdialog.cpp \
#	recipeviewdialog.cpp selectrecipedialog.cpp \
#	ingredientsdialog.cpp selectunitdialog.cpp \
#	createelementdialog.cpp propertiesdialog.cpp \
#	createpropertydialog.cpp selectpropertydialog.cpp \
#	unitsdialog.cpp dependanciesdialog.cpp \
#	shoppinglistdialog.cpp shoppinglistviewdialog.cpp \
#	selectcategoriesdialog.cpp categorieseditordialog.cpp \
#	authorsdialog.cpp selectauthorsdialog.cpp \
#	resizerecipedialog.cpp \
#	dietviewdialog.cpp ingredientmatcherdialog.cpp \
#	prepmethodsdialog.cpp \
#	createcategorydialog.cpp borderdialog.cpp \
#	refineshoppinglistdialog.cpp pagesetupdialog.cpp \
#	dbimportdialog.cpp createunitdialog.cpp \
#	setupdisplay.cpp \
#	ingredientparserdialog.cpp ingredientgroupsdialog.cpp \
#	editratingdialog.cpp similarcategoriesdialog.cpp \
#	conversiondialog.cpp createingredientweightdialog.cpp \
#	recipeprintpreview.cpp editpropertiesdialog.cpp \
#	createunitconversiondialog.cpp
#
#libkrecipesdialogs_la_METASOURCES=AUTO
#
##the library search path.
#libkrecipesdialogs_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
