########### QT3Support ###############

add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

######################################


include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${CMAKE_CURRENT_SOURCE_DIR}/../ )

if ( link_lib_SQLITE )
   add_subdirectory( SQLite )
endif ( link_lib_SQLITE )

if ( link_lib_MYSQL )
   add_subdirectory( MySQL )
endif ( link_lib_MYSQL )

if ( link_lib_POSTGRESQL ) 
   add_subdirectory( PostgreSQL )
endif ( link_lib_POSTGRESQL )

set(krecipesdbs_SRCS
   recipedb.cpp
   qsqlrecipedb.cpp
   progressinterface.cpp
   usda_property_data.cpp
   usda_ingredient_data.cpp
   usda_unit_data.cpp)

kde4_add_library(krecipesdbs SHARED ${krecipesdbs_SRCS})

target_link_libraries(krecipesdbs ${KDE4_KDECORE_LIBS} ${QT_QTGUI_LIBRARY} ${QT_QTSQL_LIBRARY} ${QT_QT3SUPPORT_LIBRARY} )


########### install files ###############




#original Makefile.am contents follow:

### Makefile.am for krecipes
#
## this is the program that gets installed.  it's name is used for all
## of the other Makefile.am variables
#
## set the include path for X, qt and KDE
#INCLUDES = -I$(srcdir) -I$(srcdir)/.. $(all_includes)
#
## Check for optional database libs
#
#
#if link_lib_SQLITE
#sqlite_subdirs=SQLite
#sqlite_libs=SQLite/libkrecsqlite.la
#endif
#
#if link_lib_MYSQL
#mysql_subdirs=MySQL
#mysql_libs=MySQL/libkrecmysql.la
#endif
#
#if link_lib_POSTGRESQL
#psql_subdirs=PostgreSQL
#psql_libs=PostgreSQL/libkrecpsql.la
#endif
#
## Optional subdirectories
#
#SUBDIRS=$(sqlite_subdirs) $(mysql_subdirs) $(psql_subdirs)
#
## Instructions for building the convenience library
#noinst_LTLIBRARIES=libkrecipesdbs.la
#libkrecipesdbs_la_SOURCES=recipedb.cpp qsqlrecipedb.cpp progressinterface.cpp usda_property_data.cpp \
#	usda_ingredient_data.cpp usda_unit_data.cpp
#libkrecipesdbs_la_METASOURCES=AUTO
#
#libkrecipesdbs_la_LIBADD= $(mysql_libs) $(sqlite_libs) $(psql_libs)
#
##the library search path.
#libkrecipesdbs_la_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#
##install the following headers
## include_HEADERS = recipedb.h
