
include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  DESTINATION ${DATA_INSTALL_DIR}/krecipes/icons )

kde4_install_icons( ${ICON_INSTALL_DIR} )



#original Makefile.am contents follow:

#krecipesdir   = $(kde_datadir)/krecipes/icons
#krecipes_ICON = AUTO
#
#KDE_ICON = krecipes_file
#
