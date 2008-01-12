
include_directories( ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############

install( FILES  None.klo  	    Default.xsl Default.css Default.png  	    Two_Column.xsl Two_Column.css Two_Column.png  	    Simple_Orange.xsl Simple_Orange.css Simple_Orange.png  	    Simple_Green.xsl Simple_Green.css box_bottom.gif box_top.gif Simple_Green.png  	    Book.xsl Book.css paper.gif Book.png DESTINATION ${DATA_INSTALL_DIR}/krecipes/layouts )



#original Makefile.am contents follow:

#basedir = $(kde_datadir)/krecipes/layouts
#base_DATA = None.klo \
#	    Default.xsl Default.css Default.png \
#	    Two_Column.xsl Two_Column.css Two_Column.png \
#	    Simple_Orange.xsl Simple_Orange.css Simple_Orange.png \
#	    Simple_Green.xsl Simple_Green.css box_bottom.gif box_top.gif Simple_Green.png \
#	    Book.xsl Book.css paper.gif Book.png
