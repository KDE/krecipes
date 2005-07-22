## Makefile.am for krecipes

# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = krecipes

SUBDIRS = backends importers dialogs exporters widgets datablocks

# set the include path for X, qt and KDE
INCLUDES         = -I$(srcdir) -I$(srcdir)/backends -I$(srcdir)/backends/SQLite $(all_includes)

# the library search path.
krecipes_LDFLAGS = $(KDE_RPATH) $(all_libraries)


# Check for optional libs
if link_lib_SQLITE
qsqlite_libadds = backends/SQLite/libkrecsqlite.la $(SQLITE_LIB)
endif

if link_lib_MYSQL
mysql_libadds =backends/MySQL/libkrecmysql.la
endif

if link_lib_POSTGRESQL
psql_libadds =backends/PostgreSQL/libkrecpsql.la
endif

# the shared libraries to link against.
krecipes_LDADD = \
	backends/libkrecipesdbs.la exporters/libkrecipesexporters.la \
	importers/libkrecipesimporters.la dialogs/libkrecipesdialogs.la \
	widgets/libkrecipeswidgets.la datablocks/libdatablocks.la \
	$(qsqlite_libadds) $(mysql_libadds) $(psql_libadds) $(LIB_KHTML) $(LIB_KSPELL)

# which sources should be compiled for krecipes
krecipes_SOURCES = \
	main.cpp krecipes.cpp krecipesview.cpp pref.cpp \
	krecipesiface.skel krecipesdbiface.skel \
	propertycalculator.cpp setupwizard.cpp \
	shoppingcalculator.cpp kstartuplogo.cpp \
	mixednumber.cpp recipeactionshandler.cpp \
	recipefilter.cpp krepagelayout.cpp kreunit.cpp \
	convert_sqlite3.cpp

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

KDE_ICON = krecipes

# this is where the kdelnk file will go
kdelnkdir   = $(kde_appsdir)/Utilities
kdelnk_DATA = krecipes.desktop

# this is where the XML-GUI resource file goes
rcdir = $(kde_datadir)/krecipes
rc_DATA = krecipesui.rc
