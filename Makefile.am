SUBDIRS = src icons pics data

messages: rc.cpp
	$(EXTRACTRC) `find ./src/ -name "*.ui"`  > rc.cpp
	$(XGETTEXT) `find ./src/ -name "*.cpp"` -o $(podir)/krecipes.pot
	rm -f rc.cpp
