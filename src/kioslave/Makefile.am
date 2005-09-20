INCLUDES    = -I../backends/ -I../ \
		$(all_includes) 

METASOURCES = AUTO
KDE_CFLAGS =  -w

kde_module_LTLIBRARIES = kio_krecipes.la

kio_krecipes_la_SOURCES = krecipes.cpp
kio_krecipes_la_LIBADD = $(SQLITE_LIB) $(LIB_KIO) ../datablocks/libdatablocks.la ../backends/libkrecipesdbs.la
kio_krecipes_la_LDFLAGS = -module -avoid-version $(all_libraries) \
	$(KDE_PLUGIN)

protocol_DATA = krecipes.protocol
protocoldir = $(kde_servicesdir)
