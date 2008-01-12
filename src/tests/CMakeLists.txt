
include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############

set(kretest_SRCS kretest.cpp )

if(KDE4_BUILD_TESTS)

kde4_add_executable(kretest ${kretest_SRCS})

target_link_libraries(kretest  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(mmftest_SRCS mmftest.cpp )

if(KDE4_BUILD_TESTS)

kde4_add_executable(mmftest ${mmftest_SRCS})

target_link_libraries(mmftest  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(mx2test_SRCS mx2test.cpp )

if(KDE4_BUILD_TESTS)

kde4_add_executable(mx2test ${mx2test_SRCS})

target_link_libraries(mx2test  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(mxptest_SRCS mxptest.cpp )

if(KDE4_BUILD_TESTS)

kde4_add_executable(mxptest ${mxptest_SRCS})

target_link_libraries(mxptest  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(rezkonvtest_SRCS rezkonvtest.cpp )

if(KDE4_BUILD_TESTS)

kde4_add_executable(rezkonvtest ${rezkonvtest_SRCS})

target_link_libraries(rezkonvtest  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(nyctest_SRCS nyctest.cpp )

if(KDE4_BUILD_TESTS)

kde4_add_executable(nyctest ${nyctest_SRCS})

target_link_libraries(nyctest  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### next target ###############

set(recipemltest_SRCS recipemltest.cpp )

if(KDE4_BUILD_TESTS)

kde4_add_executable(recipemltest ${recipemltest_SRCS})

target_link_libraries(recipemltest  ${KDE4_KDECORE_LIBS} )

endif(KDE4_BUILD_TESTS)

########### install files ###############




#original Makefile.am contents follow:

#INCLUDES = -I$(srcdir)/.. -I$(srcdir)/../importers -I$(srcdir)/../exporters $(all_includes)
#
#AM_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#
#check_PROGRAMS = kretest mmftest mx2test mxptest rezkonvtest nyctest recipemltest
#
#noinst_HEADERS = importertest.h exportertest.h
#
#METASOURCES = AUTO
#
#LDADD = ../importers/libkrecipesimporters.la ../exporters/libkrecipesexporters.la ../backends/libkrecipesdbs.la ../datablocks/libdatablocks.la $(LIB_QT) $(LIB_KHTML) $(LIB_KSPELL)
#
#kretest_SOURCES = kretest.cpp
#mmftest_SOURCES = mmftest.cpp
#mx2test_SOURCES = mx2test.cpp
#mxptest_SOURCES = mxptest.cpp
#rezkonvtest_SOURCES = rezkonvtest.cpp
#nyctest_SOURCES = nyctest.cpp
#recipemltest_SOURCES = recipemltest.cpp
