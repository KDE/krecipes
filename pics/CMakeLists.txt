
include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  network.png save.png wizard.png dbpermissions.png recipes.png startlogo.png menubg.png  	star_on.png  	star_off.png  	star_half.png  	rating0.png rating1.png rating2.png rating3.png rating4.png rating5.png DESTINATION ${DATA_INSTALL_DIR}/krecipes/pics )



#original Makefile.am contents follow:

#picsdir = $(kde_datadir)/krecipes/pics
#pics_DATA = network.png save.png wizard.png dbpermissions.png recipes.png startlogo.png menubg.png \
#	star_on.png \
#	star_off.png \
#	star_half.png \
#	rating0.png rating1.png rating2.png rating3.png rating4.png rating5.png
#
