SUBDIRS = src icons pics data po

messages:
	$(XGETTEXT) src/*.cpp src/*.h -o po/krecipes.pot
package-messages:
	$(MAKE) -C po merge
