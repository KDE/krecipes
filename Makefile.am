SUBDIRS = src icons pics data layouts

messages: rc.cpp
	find ./src/ -name \*.ui | xargs --no-run-if-empty $(EXTRACTRC) >> rc.cpp
	$(XGETTEXT) rc.cpp `find ./src/ -name \*.cpp` -o $(podir)/krecipes.pot
	rm -f rc.cpp
