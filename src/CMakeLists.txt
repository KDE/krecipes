
include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

add_subdirectory( backends )
add_subdirectory( importers )
add_subdirectory( widgets )
add_subdirectory( dialogs )
add_subdirectory( exporters )
add_subdirectory( datablocks )
add_subdirectory( tests )



########### QT3Support ###############

add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)


########### next target ###############

set(krecipes_SRCS 
   main.cpp 
   krecipes.cpp 
   krecipesview.cpp 
   pref.cpp 
   propertycalculator.cpp 
   setupwizard.cpp 
   shoppingcalculator.cpp 
   kstartuplogo.cpp 
   recipeactionshandler.cpp 
   recipefilter.cpp 
   convert_sqlite3.cpp 
   klomanager.cpp )

#kde4_add_dcop_skels(krecipes_SRCS );
#MESSAGE(STATUS "DCOP stuff was remove into kde4, convert it to dbus please.kde4_add_dcop_skels macro doesn't exist now. We keep it just to remember to port to dbus")
##########KDE4 PORT
##########krecipesiface.h krecipesdbiface.h )

kde4_add_executable(krecipes ${krecipes_SRCS})

target_link_libraries(krecipes  ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} krecipesdbs krecipesexporters krecipesimporters krecipeswidgets krecipesdialogs krecipeswidgets datablocks ${KDE4_KHTML_LIBS} )

install(TARGETS krecipes DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

install( FILES  krecipesui.rc DESTINATION ${DATA_INSTALL_DIR}/krecipes )
install( FILES  krecipes.desktop DESTINATION ${APPLNK_INSTALL_DIR}/Utilities )

kde4_install_icons( ${ICON_INSTALL_DIR} )



#original Makefile.am contents follow:

### Makefile.am for krecipes
#
## this is the program that gets installed.  it's name is used for all
## of the other Makefile.am variables
#bin_PROGRAMS = krecipes
#
#SUBDIRS = backends importers widgets dialogs exporters datablocks tests
#
## set the include path for X, qt and KDE
#INCLUDES         = -I$(srcdir) -I$(srcdir)/backends -I$(srcdir)/backends/SQLite $(all_includes)
#
## the library search path.
#krecipes_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#
#
## Check for optional libs
#if link_lib_SQLITE
#qsqlite_libadds = backends/SQLite/libkrecsqlite.la $(SQLITE_LIB)
#endif
#
#if link_lib_MYSQL
#mysql_libadds =backends/MySQL/libkrecmysql.la
#endif
#
#if link_lib_POSTGRESQL
#psql_libadds =backends/PostgreSQL/libkrecpsql.la
#endif
#
## the shared libraries to link against.
#krecipes_LDADD = \
#	backends/libkrecipesdbs.la exporters/libkrecipesexporters.la \
#	importers/libkrecipesimporters.la widgets/libkrecipeswidgets.la dialogs/libkrecipesdialogs.la \
#	widgets/libkrecipeswidgets.la datablocks/libdatablocks.la \
#	$(qsqlite_libadds) $(mysql_libadds) $(psql_libadds) $(LIB_KHTML) $(LIB_KSPELL)
#
## which sources should be compiled for krecipes
#krecipes_SOURCES = \
#	main.cpp krecipes.cpp krecipesview.cpp pref.cpp \
#	krecipesiface.skel krecipesdbiface.skel \
#	propertycalculator.cpp setupwizard.cpp \
#	shoppingcalculator.cpp kstartuplogo.cpp \
#	recipeactionshandler.cpp \
#	recipefilter.cpp \
#	convert_sqlite3.cpp klomanager.cpp
#
## let automoc handle all of the meta source files (moc)
#METASOURCES = AUTO
#
#KDE_ICON = krecipes
#
## this is where the kdelnk file will go
#kdelnkdir   = $(kde_appsdir)/Utilities
#kdelnk_DATA = krecipes.desktop
#
## this is where the XML-GUI resource file goes
#rcdir = $(kde_datadir)/krecipes
#rc_DATA = krecipesui.rc
