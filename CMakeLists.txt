project(krecipes)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake 
                      ${CMAKE_MODULE_PATH})

# search packages used by KDE

find_package(KDE4 REQUIRED)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

include(KDE4Defaults)

include(MacroLibrary)

include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )

add_subdirectory( src )
add_subdirectory( icons )
add_subdirectory( pics )
add_subdirectory( data )
add_subdirectory( layouts )
add_subdirectory( mimetypes )
add_subdirectory( doc )

