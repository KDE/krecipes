include(FindLibXml2)
include(FindLibXslt)
include(FindQImageBlitz)

find_package(Sqlite)
find_package(PostgreSQL)
find_package(MySQL)
find_package(QImageBlitz)
include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/backends/ ${QIMAGEBLITZ_INCLUDES})

########### QT3Support ###############

add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

############# backends ###############


include(ConfigureChecks.cmake)
configure_file(config-krecipes.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-krecipes.h )

if ( SQLITE_FOUND )
   include_directories(${SQLITE_INCLUDE_DIR})
   set(sqlite_SRCS  backends/SQLite/libqsqlite/krecqsqlitedb.cpp backends/SQLite/libqsqlite/krecqsqliteresult.cpp backends/SQLite/literecipedb.cpp  backends/SQLite/qsql_sqlite.cpp)
endif ( SQLITE_FOUND )

if ( MYSQL_FOUND )
   set(mysql_SRCS backends/MySQL/mysqlrecipedb.cpp)
endif ( MYSQL_FOUND )

if ( POSTGRESQL_FOUND )
   set(postgresql_SRCS backends/PostgreSQL/psqlrecipedb.cpp )
endif ( POSTGRESQL_FOUND )

set(krecipesdbs_SRCS
   backends/recipedb.cpp
   backends/qsqlrecipedb.cpp
   backends/progressinterface.cpp
   backends/usda_property_data.cpp
   backends/usda_ingredient_data.cpp
   backends/usda_unit_data.cpp)

qt4_add_dbus_adaptor(krecipesdbs_SRCS backends/org.kde.krecipesdb.xml recipedb.h RecipeDB)


############ importers ###############

set(krecipesimporters_SRCS
   importers/mx2importer.cpp
   importers/mmfimporter.cpp
   importers/mxpimporter.cpp
   importers/nycgenericimporter.cpp
   importers/recipemlimporter.cpp
   importers/baseimporter.cpp
   importers/kreimporter.cpp
   importers/rezkonvimporter.cpp
   importers/kredbimporter.cpp)


############### widgets ##############

set(krecipeswidgets_SRCS
   widgets/krelistview.cpp
   widgets/kremenu.cpp
   widgets/paneldeco.cpp
   widgets/ingredientlistview.cpp
   widgets/unitlistview.cpp
   widgets/propertylistview.cpp
   widgets/prepmethodlistview.cpp
   widgets/categorylistview.cpp
   widgets/authorlistview.cpp
   widgets/recipelistview.cpp
   widgets/categorycombobox.cpp
   widgets/kretextedit.cpp
   widgets/dblistviewbase.cpp
   widgets/conversiontable.cpp
   widgets/fractioninput.cpp
   widgets/ingredientcombobox.cpp
   widgets/headercombobox.cpp
   widgets/prepmethodcombobox.cpp
   widgets/inglistviewitem.cpp
   widgets/kdateedit.cpp
   widgets/kdatepickerpopup.cpp
   widgets/headerlistview.cpp
   widgets/ratingwidget.cpp
   widgets/kwidgetlistbox.cpp
   widgets/criteriacombobox.cpp
   widgets/ingredientinputwidget.cpp
   widgets/unitcombobox.cpp
   widgets/amountunitinput.cpp
   widgets/weightinput.cpp
   widgets/thumbbar.cpp)


############## dialogs ###############

set(krecipesdialogs_SRCS
   dialogs/advancedsearchdialog.cpp
   dialogs/recipeimportdialog.cpp
   dialogs/dietwizarddialog.cpp
   dialogs/recipeinputdialog.cpp
   dialogs/recipeviewdialog.cpp
   dialogs/selectrecipedialog.cpp
   dialogs/ingredientsdialog.cpp
   dialogs/selectunitdialog.cpp
   dialogs/createelementdialog.cpp
   dialogs/propertiesdialog.cpp
   dialogs/createpropertydialog.cpp
   dialogs/selectpropertydialog.cpp
   dialogs/unitsdialog.cpp
   dialogs/dependanciesdialog.cpp
   dialogs/shoppinglistdialog.cpp
   dialogs/shoppinglistviewdialog.cpp
   dialogs/selectcategoriesdialog.cpp
   dialogs/categorieseditordialog.cpp
   dialogs/authorsdialog.cpp
   dialogs/selectauthorsdialog.cpp
   dialogs/resizerecipedialog.cpp
   dialogs/dietviewdialog.cpp
   dialogs/ingredientmatcherdialog.cpp
   dialogs/prepmethodsdialog.cpp
   dialogs/createcategorydialog.cpp
   dialogs/borderdialog.cpp
   dialogs/refineshoppinglistdialog.cpp
   dialogs/pagesetupdialog.cpp
   dialogs/dbimportdialog.cpp
   dialogs/createunitdialog.cpp
   dialogs/setupdisplay.cpp
   dialogs/ingredientparserdialog.cpp
   dialogs/ingredientgroupsdialog.cpp
   dialogs/editratingdialog.cpp
   dialogs/similarcategoriesdialog.cpp
   dialogs/conversiondialog.cpp
   dialogs/createingredientweightdialog.cpp
   dialogs/recipeprintpreview.cpp
   dialogs/editpropertiesdialog.cpp
   dialogs/createunitconversiondialog.cpp)


############# exporters ##############

set (krecipesexporters_SRCS 
   exporters/kreexporter.cpp
   exporters/baseexporter.cpp
   exporters/cookmlexporter.cpp
   exporters/recipemlexporter.cpp
   exporters/mmfexporter.cpp
   exporters/htmlexporter.cpp
   exporters/plaintextexporter.cpp
   exporters/rezkonvexporter.cpp
   exporters/htmlbookexporter.cpp
   exporters/xsltexporter.cpp
   )


############ datablocks ##############

set(datablocks_SRCS
   datablocks/recipelist.cpp
   datablocks/constraintlist.cpp
   datablocks/categorytree.cpp
   datablocks/kreborder.cpp
   datablocks/recipe.cpp
   datablocks/ingredient.cpp
   datablocks/ingredientlist.cpp
   datablocks/elementlist.cpp
   datablocks/element.cpp
   datablocks/ingredientproperty.cpp
   datablocks/ingredientpropertylist.cpp
   datablocks/unit.cpp
   datablocks/unitratio.cpp
   datablocks/unitratiolist.cpp
   datablocks/mixednumber.cpp
   datablocks/rating.cpp
   datablocks/weight.cpp)


############### tests ################

add_subdirectory( tests )


############# Main  #################

set(krecipes_SRCS 
   main.cpp 
   krecipes.cpp 
   krecipesview.cpp 
   pref.cpp 
   propertycalculator.cpp 
   setupassistant.cpp 
   shoppingcalculator.cpp 
   kstartuplogo.cpp 
   recipeactionshandler.cpp 
   recipefilter.cpp 
   convert_sqlite3.cpp 
   klomanager.cpp )


kde4_add_ui_files(krecipes_SRCS widgets/ratingdisplaywidget.ui)

qt4_add_dbus_adaptor(krecipes_SRCS org.kde.krecipes.xml krecipesview.h KrecipesView)


kde4_add_executable(krecipes ${mysql_SRCS} ${postgresql_SRCS} ${sqlite_SRCS} ${krecipes_SRCS} ${krecipesdbs_SRCS} ${krecipesimporters_SRCS} ${krecipeswidgets_SRCS}   ${krecipesexporters_SRCS} ${datablocks_SRCS} ${krecipesdialogs_SRCS})

target_link_libraries(krecipes  ${KDE4_KHTML_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KDE3SUPPORT_LIBS} ${QT_QTSQL_LIBRARY} ${LIBXSLT_LIBRARIES} ${QIMAGEBLITZ_LIBRARIES})

if ( SQLITE_FOUND )
   target_link_libraries(krecipes ${SQLITE_LIBRARIES})
endif ( SQLITE_FOUND )

if ( MYSQL_FOUND )
   target_link_libraries(krecipes ${MYSQL_LIBRARIES})
endif ( MYSQL_FOUND )

if ( POSTGRESQL_FOUND )
   target_link_libraries(krecipes ${POSTGRESQL_LIBRARIES})
endif ( POSTGRESQL_FOUND )


install(TARGETS krecipes ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES  krecipesui.rc DESTINATION ${DATA_INSTALL_DIR}/krecipes )
install( FILES  krecipes.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )

kde4_install_icons( ${ICON_INSTALL_DIR} )


