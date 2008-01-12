
include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  x-krecipes-backup.desktop x-krecipes-recipes.desktop DESTINATION ${MIME_INSTALL_DIR}/application )
install( FILES  data-en_US.sql abbrev.txt weight.txt samples-en_US.kreml samples-cs_CZ.kreml property-data-en_US.txt ingredient-data-en_US.txt unit-data-en_US.txt prep-data-en_US.txt DESTINATION ${DATA_INSTALL_DIR}/krecipes/data )



#original Makefile.am contents follow:

#basedir = $(kde_datadir)/krecipes/data
#base_DATA = data-en_US.sql abbrev.txt weight.txt samples-en_US.kreml samples-cs_CZ.kreml property-data-en_US.txt ingredient-data-en_US.txt unit-data-en_US.txt prep-data-en_US.txt
#
#mimetypedir = $(kde_mimedir)/application
#mimetype_DATA = x-krecipes-backup.desktop x-krecipes-recipes.desktop
