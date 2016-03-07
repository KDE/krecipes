find_package(LibXml2 REQUIRED)
find_package(LibXslt REQUIRED)
include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}
   ${LIBXML2_INCLUDE_DIR} ${LIBXSLT_INCLUDE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/backends/
   ${CMAKE_CURRENT_SOURCE_DIR}/threadedqueries
   ${CMAKE_CURRENT_SOURCE_DIR}/importers
   ${CMAKE_CURRENT_SOURCE_DIR}/exporters
   ${CMAKE_CURRENT_SOURCE_DIR}/actionshandlers
   ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

########### QT3Support ###############

add_definitions (-DQT3_SUPPORT -DQT3_SUPPORT_WARNINGS)

############# backends ###############

set(sqlite_SRCS backends/SQLite/literecipedb.cpp)

set(mysql_SRCS backends/MySQL/mysqlrecipedb.cpp)

set(postgresql_SRCS backends/PostgreSQL/psqlrecipedb.cpp )

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

##### Threaded database queries  #####

set(krecipesthreadedqueries_SRCS
   threadedqueries/loadingauthorsthread.cpp
)

######### Actions Handlers ###########

set(krecipesactionshandlers_SRCS
   actionshandlers/krerecipeactionshandler.cpp
   actionshandlers/kregenericactionshandler.cpp
   actionshandlers/kreingredientactionshandler.cpp
   actionshandlers/kreheaderactionshandler.cpp
   actionshandlers/krepropertyactionshandler.cpp
   actionshandlers/kreunitactionshandler.cpp
   actionshandlers/kreprepmethodactionshandler.cpp
   actionshandlers/krecategoryactionshandler.cpp
   actionshandlers/kreauthoractionshandler.cpp

   #FIXME: The classes below are deprecated and must be removed sooner or later.
   actionshandlers/actionshandlerbase.cpp
   actionshandlers/categoryactionshandler.cpp
   actionshandlers/unitactionshandler.cpp
   actionshandlers/propertyactionshandler.cpp
   actionshandlers/prepmethodactionshandler.cpp
   actionshandlers/ingredientactionshandler.cpp
   actionshandlers/headeractionshandler.cpp
   actionshandlers/recipeactionshandler.cpp
)

############### widgets ##############

set(krecipeswidgets_SRCS
   widgets/clickablelabel.cpp
   widgets/imagedroplabel.cpp
   widgets/krelistview.cpp
   widgets/kremenu.cpp
   widgets/paneldeco.cpp
   widgets/kreingredientlistwidget.cpp
   widgets/kreunitlistwidget.cpp
   widgets/krepropertylistwidget.cpp
   widgets/kreprepmethodlistwidget.cpp
   widgets/krecategorieslistwidget.cpp
   widgets/kreauthorlistwidget.cpp
   widgets/krerecipeslistwidget.cpp
   widgets/categorycombobox.cpp
   widgets/kretextedit.cpp
   widgets/kregenericlistwidget.cpp
   widgets/kresearchresultlistwidget.cpp
   widgets/kreconversiontable.cpp
   widgets/fractioninput.cpp
   widgets/ingredientcombobox.cpp
   widgets/headercombobox.cpp
   widgets/prepmethodcombobox.cpp
   widgets/inglistviewitem.cpp
   widgets/kdateedit.cpp
   widgets/kdatepickerpopup.cpp
   widgets/kreheaderlistwidget.cpp
   widgets/criteriacombobox.cpp
   widgets/ingredientinputwidget.cpp
   widgets/unitcombobox.cpp
   widgets/unittypedelegate.cpp
   widgets/amountunitinput.cpp
   widgets/amountunitinput.cpp
   widgets/weightinput.cpp
   widgets/thumbbar.cpp

   #FIXME: The classes below are deprecated and must be removed sooner or later.
   widgets/authorlistview.cpp #replaced by kreauthorlistwidget.cpp
   widgets/dblistviewbase.cpp #replaced by kregenericlistwidget.cpp
   widgets/categorylistview.cpp #replaced by krecategorieslistwidget.cpp
   widgets/conversiontable.cpp #replaced by kreconversiontable.cpp
   widgets/unitlistview.cpp #replaced by kreunitlistwidget.cpp
   widgets/propertylistview.cpp #replaced by krepropertylistwidget.cpp
   widgets/prepmethodlistview.cpp #replaced by kreprepmethodlistwidget.cpp
   widgets/ingredientlistview.cpp #replaced by kreingredientlistwidget.cpp
   widgets/headerlistview.cpp #replaced by kreheaderlistwidget.cpp
   widgets/recipelistview.cpp

)

kde4_add_ui_files(krecipeswidgets_SRCS
   widgets/kregenericlistwidget.ui
)

############## dialogs ###############

set(krecipesdialogs_SRCS
   dialogs/advancedsearchdialog.cpp
   dialogs/recipeimportdialog.cpp

   #Diet wizard
   dialogs/dietwizard/dietwizarddialog.cpp
   dialogs/dietwizard/dietviewdialog.cpp
   dialogs/dietwizard/mealinput.cpp
   dialogs/dietwizard/dishinput.cpp
   dialogs/dietwizard/dishtitle.cpp

   #Recipe input
   dialogs/recipeinput/recipeinputdialog.cpp
   dialogs/recipeinput/recipegeneralinfoeditor.cpp
   dialogs/recipeinput/selectauthorsdialog.cpp
   dialogs/recipeinput/resizerecipedialog.cpp
   dialogs/recipeinput/editratingdialog.cpp
   dialogs/recipeinput/ingredientparserdialog.cpp
   dialogs/recipeinput/selectcategoriesdialog.cpp
   dialogs/recipeinput/ingredientseditor.cpp
   dialogs/recipeinput/ingredientnamedelegate.cpp
   dialogs/recipeinput/amountdelegate.cpp
   dialogs/recipeinput/ratingdisplaywidget.cpp
   dialogs/recipeinput/ratinglisteditor.cpp

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
   dialogs/categorieseditordialog.cpp
   dialogs/authorsdialog.cpp
   dialogs/ingredientmatcherdialog.cpp
   dialogs/prepmethodsdialog.cpp
   dialogs/createcategorydialog.cpp
   dialogs/borderdialog.cpp
   dialogs/refineshoppinglistdialog.cpp
   dialogs/pagesetupdialog.cpp
   dialogs/dbimportdialog.cpp
   dialogs/createunitdialog.cpp
   dialogs/setupdisplay.cpp
   dialogs/ingredientgroupsdialog.cpp
   dialogs/similarcategoriesdialog.cpp
   dialogs/conversiondialog.cpp
   dialogs/createingredientweightdialog.cpp
   dialogs/editpropertiesdialog.cpp
   dialogs/createunitconversiondialog.cpp)

kde4_add_ui_files(krecipesdialogs_SRCS
   dialogs/recipeinput/recipegeneralinfoeditor.ui
   dialogs/recipeinput/resizerecipedialog.ui
   dialogs/recipeinput/ratingdisplaywidget.ui
   dialogs/recipeinput/ingredientseditor.ui
   )

qt4_add_resources(krecipesdialogs_SRCS
   ../pics/resources.qrc
   )


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
   exporters/mx2exporter.cpp
   )

qt4_add_resources(krecipesexporters_SRCS
   ../pics/resources.qrc
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
   datablocks/mixednumberrange.cpp
   datablocks/rating.cpp
   datablocks/ratingcriteria.cpp
   datablocks/weight.cpp
   datablocks/yield.cpp
   )


############ validators ##############

set(validators_SRCS
   validators/mixednumbervalidator.cpp
   )


############### common library ################
# The reason to have this static library is to compile some files
# once for *all* executables in this project.
# (i.e. I want to save CPU time when compiling with test programs)

kde4_add_library(krecipescommon STATIC
   ${mysql_SRCS} ${postgresql_SRCS} ${sqlite_SRCS} ${krecipesdbs_SRCS}
   ${krecipesimporters_SRCS} ${krecipesexporters_SRCS}
   ${datablocks_SRCS} ${krecipesthreadedqueries_SRCS}
   ${validators_SRCS}
   ${krecipesactionshandlers_SRCS}
   ${krecipeswidgets_SRCS}
   ${krecipesdialogs_SRCS}
   klomanager.cpp propertycalculator.cpp)


target_link_libraries(krecipescommon
   ${KDE4_KDECORE_LIBS} ${KDE4_KDE3SUPPORT_LIBS}
   ${QT_QTSQL_LIBRARY}
   ${LIBXSLT_LIBRARIES} ${LIBXML2_LIBRARIES}
   ${QT_QTWEBKIT_LIBRARY} ${KDE4_KDEWEBKIT_LIBS}
   )

############### tests ################


set(IMPORT_EXPORT_TESTS
    kretest mmftest mx2test mxptest nyctest recipemltest rezkonvtest
    )

set(DATABLOCKS_TESTS
    rating
    ratingcriteria
    yield
    unit
    unitratio
    unitratiolist
    weight
    mixednumber
    )

set(DATABASE_TESTS
    authors
    encoding
    )

set(UI_TESTS
    ingredientseditor
    )

option(TESTS_MYSQL "Enable tests on MySQL" OFF)
option(TESTS_POSTGRESQL "Enable tests on PostgreSQL" OFF)

if(TESTS_MYSQL)
  add_definitions(-DKRE_TESTS_MYSQL)
endif()
if(TESTS_POSTGRESQL)
  add_definitions(-DKRE_TESTS_POSTGRESQL)
endif()

macro(add_import_export_test _TEST)
    if(KDE4_BUILD_TESTS)
        kde4_add_executable(krecipes-import-export-${_TEST} tests/import-export/${_TEST}.cpp)
        target_link_libraries(krecipes-import-export-${_TEST} ${UNIT_TEST_LIBRARIES})

        set(_TEST_DATA
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/import-export/${_TEST}.txt
            ${CMAKE_CURRENT_SOURCE_DIR}/tests/import-export/test_photo.jpg)

        add_test(NAME krecipes-import-export-${_TEST}
            COMMAND $<TARGET_FILE:krecipes-import-export-${_TEST}> ${_TEST_DATA})
    endif(KDE4_BUILD_TESTS)
endmacro(add_import_export_test)

macro(add_datablocks_test _TEST)
    kde4_add_unit_test(krecipes-datablocks-${_TEST} tests/datablocks/${_TEST}.cpp)
    target_link_libraries(krecipes-datablocks-${_TEST} ${UNIT_TEST_LIBRARIES})
endmacro(add_datablocks_test)

macro(add_database_test _TEST)
    set(krecipes-database-${_TEST}_SRCS
        tests/database/${_TEST}.cpp)
    qt4_add_resources(krecipes-database-${_TEST}_SRCS
        tests/database/dbconfigs.qrc)
    kde4_add_unit_test(krecipes-database-${_TEST}
         ${krecipes-database-${_TEST}_SRCS})
    target_link_libraries(krecipes-database-${_TEST} ${UNIT_TEST_LIBRARIES})
endmacro(add_database_test)

macro(add_ui_test _TEST)
    kde4_add_unit_test(krecipes-ui-${_TEST} tests/ui/${_TEST}.cpp)
    target_link_libraries(krecipes-ui-${_TEST} ${UNIT_TEST_LIBRARIES})
endmacro(add_ui_test)

set(UNIT_TEST_LIBRARIES krecipescommon
    ${KDE4_KDECORE_LIBS} ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QTSQL_LIBRARY}  ${QT_QTTEST_LIBRARY}
    ${LIBXSLT_LIBRARIES} ${LIBXML2_LIBRARIES}
    )

foreach(_TEST ${IMPORT_EXPORT_TESTS})
    add_import_export_test(${_TEST})
endforeach(_TEST)
foreach(_TEST ${DATABLOCKS_TESTS})
    add_datablocks_test(${_TEST})
endforeach(_TEST)
foreach(_TEST ${DATABASE_TESTS})
    add_database_test(${_TEST})
endforeach(_TEST)
foreach(_TEST ${UI_TESTS})
    add_ui_test(${_TEST})
endforeach(_TEST)


############# NukeDB #################

if(BUILD_NUKEDB)
   set(nukedb_SRCS
      nukedb/main.cpp
      nukedb/mainwindow.cpp
      nukedb/nukeconfigwidget.cpp
   )

   kde4_add_ui_files(nukedb_SRCS
      nukedb/nukeconfigwidget.ui
   )

   kde4_add_executable(nukedb ${nukedb_SRCS})
   target_link_libraries(nukedb krecipescommon ${KDE4_KDEUI_LIBS})

   install(TARGETS nukedb ${INSTALL_TARGETS_DEFAULT_ARGS})
endif(BUILD_NUKEDB)


############# Main  #################

set(krecipes_SRCS
   main.cpp
   krecipes.cpp
   krecipesview.cpp
   pref.cpp
   #propertycalculator.cpp
   setupassistant.cpp
   shoppingcalculator.cpp
   convert_sqlite3.cpp
   #klomanager.cpp
)

qt4_add_resources(krecipes_SRCS
   ../pics/resources.qrc
   )

qt4_add_dbus_adaptor(krecipes_SRCS org.kde.krecipes.xml krecipesview.h KrecipesView)


kde4_add_executable(krecipes
   ${krecipes_SRCS}
   )

target_link_libraries(krecipes krecipescommon ${KDE4_KHTML_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KDE3SUPPORT_LIBS} ${QT_QTSQL_LIBRARY} ${LIBXSLT_LIBRARIES} ${LIBXML2_LIBRARIES} ${QT_QTWEBKIT_LIBRARY} ${KDE4_KDEWEBKIT_LIBS})


install(TARGETS krecipes ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES  krecipesui.rc DESTINATION ${DATA_INSTALL_DIR}/krecipes )
install( PROGRAMS  krecipes.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )

kde4_install_icons( ${ICON_INSTALL_DIR} )


