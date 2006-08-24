## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables

# set the include path for X, qt and KDE
INCLUDES         = -I$(srcdir)/.. $(all_includes)

noinst_LTLIBRARIES=libkrecipeswidgets.la

libkrecipeswidgets_la_SOURCES= \
	krelistview.cpp kremenu.cpp \
	paneldeco.cpp ingredientlistview.cpp unitlistview.cpp \
	propertylistview.cpp prepmethodlistview.cpp categorylistview.cpp \
	authorlistview.cpp recipelistview.cpp categorycombobox.cpp \
	kretextedit.cpp dblistviewbase.cpp \
	conversiontable.cpp fractioninput.cpp ingredientcombobox.cpp \
	headercombobox.cpp prepmethodcombobox.cpp \
	inglistviewitem.cpp kdateedit.cpp kdatepickerpopup.cpp \
	headerlistview.cpp ratingwidget.cpp kwidgetlistbox.cpp \
	ratingdisplaywidget.ui criteriacombobox.cpp ingredientinputwidget.cpp \
	unitcombobox.cpp amountunitinput.cpp weightinput.cpp


libkrecipeswidgets_la_METASOURCES=AUTO

#the library search path.
libkrecipeswidgets_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
