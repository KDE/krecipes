SUBDIRS = src icons pics data layouts

messages: rc.cpp
	$(EXTRACTRC) `find ./src/ -name "\*.ui"`  > rc.cpp
	$(XGETTEXT) rc.cpp `find ./src/ -name "\*.cpp"` -o $(podir)/krecipes.pot
	rm -f rc.cpp
