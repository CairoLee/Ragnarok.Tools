MAKE = make
CXX = g++
CC = gcc
SUBDIR = gtk-test check common pakext pakext-gtk \
	actor actor-gtk rsmview rsmview-gtk actCA
ZIPSUBDIR = actCA actor actor-gtk check common \
	pakext pakext-gtk rsmview rsmview-gtk
ZIPBIN = ArcData-bin
ZIPSRC = ArcData-src

all:
	for dir in $(SUBDIR); do \
	  $(MAKE) -C $$dir; \
	done

zip:
	make clean
	rm -f $(ZIPBIN).zip
	mv distribution $(ZIPBIN)
	zip -r $(ZIPBIN).zip $(ZIPBIN)
	mv $(ZIPBIN) distribution
	rm -f $(ZIPSRC).zip
	zip $(ZIPSRC).zip ArcData.sln LICENCE_JA licence REDISTRIBUTION_JA.txt Makefile EULA.txt EULA_comp.z
	zip -r $(ZIPSRC).zip $(ZIPSUBDIR)
	zip $(ZIPSRC).zip distribution
	zip $(ZIPSRC).zip po_dattools/jp-kr.cpp po_dattools/makemsgjp.cpp po_dattools/res2real.cpp po_dattools/cardmob.cpp
	zip $(ZIPSRC).zip po_dattools/pakviewer.l.mo.sjis po_dattools/pakviewer.l.mo.euc po_dattools/pakviewer.l.mo.utf8
	zip $(ZIPSRC).zip po_dattools/pakviewer.l.po.sjis

install:
	cp -f pakext/pakext /usr/local/bin
	cp -f actor/actor /usr/local/bin
	cp -f rsmview/rsmview /usr/local/bin
	cp -f actCA/actCA /usr/local/bin
	cp -f po_dattools/pakviewer.l.mo.utf8 /usr/local/share/locale/ja/LC_MESSAGES/pakviewer.l.mo

install2:
	cp -f pakext/pakext $(HOME)/bin
	cp -f actor/actor $(HOME)/bin
	cp -f rsmview/rsmview $(HOME)/bin
	cp -f actCA/actCA $(HOME)/bin

clean:
	for dir in $(SUBDIR); do \
	  $(MAKE) -C $$dir clean; \
	done
	rm -rf debug release $(ZIPBIN).zip $(ZIPSRC).zip .*.swp
