# - Try to find Sqlite 2.x series
# Once done this will define
#
#  SQLITE2_FOUND - system has Sqlite 2.x
#  SQLITE2_INCLUDE_DIR - the Sqlite 2.x include directory
#  SQLITE2_LIBRARIES - Link these to use Sqlite 2.x
#  SQLITE2_DEFINITIONS - Compiler switches required for using Sqlite 2.x
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


# Copyright (c) 2008, Gilles Caulier, <caulier.gilles@gmail.com>
# Copyright (c) 2009, José Manuel Santamaría Lema, <panfaust@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

# Based on FindSq1ite.cmake from Guilles Caulier.

if ( SQLITE2_INCLUDE_DIR AND SQLITE2_LIBRARIES )
   # in cache already
   SET(Sqlite2_FIND_QUIETLY TRUE)
endif ( SQLITE2_INCLUDE_DIR AND SQLITE2_LIBRARIES )

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if( NOT WIN32 )
  find_package(PkgConfig)

  pkg_check_modules(PC_SQLITE2 sqlite)

  set(SQLITE2_DEFINITIONS ${PC_SQLITE2_CFLAGS_OTHER})
endif( NOT WIN32 )

FIND_PATH(SQLITE2_INCLUDE_DIR NAMES sqlite.h
  PATHS
  ${PC_SQLITE_INCLUDEDIR}
  ${PC_SQLITE_INCLUDE_DIRS}
)

FIND_LIBRARY(SQLITE2_LIBRARIES NAMES sqlite
  PATHS
  ${PC_SQLITE_LIBDIR}
  ${PC_SQLITE_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sqlite2 DEFAULT_MSG SQLITE2_INCLUDE_DIR SQLITE2_LIBRARIES )

# show the SQLITE_INCLUDE_DIR and SQLITE_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(SQLITE2_INCLUDE_DIR SQLITE2_LIBRARIES )

