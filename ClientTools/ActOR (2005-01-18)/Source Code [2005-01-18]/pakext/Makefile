CXX = g++
CC = gcc
CFLAGS = -O3 `pkg-config gtk+-2.0 --cflags` `pkg-config gtkglext-1.0 --cflags`
LIBS = `pkg-config gtk+-2.0 --libs` `pkg-config gtkglext-1.0 --libs` -lz -lGL -lGLU -lgettextlib
SUBDIR = gtk win32
OBJS = main.o \
	chrview.o \
	gtk/chrview_gtk.o \
	gtk/easyext_gtk.o \
	gtk/mp98_gtk.o
RELOBJS = ../common/pak.o \
	../common/Sprite.o \
	../common/act.o \
	../common/spr.o \
	../common/vec.o \
	../common/getopt.o \
	../common/codeconv.o

all: pakext

pakext: $(OBJS) $(RELOBJS)
	for dir in $(SUBDIR); do \
	  $(MAKE) -C $$dir; \
	done
	$(CXX) $(CFLAGS) -o $@ $(OBJS) \
	$(RELOBJS) $(LIBS)

.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	for dir in $(SUBDIR); do \
	  $(MAKE) -C $$dir clean; \
	done
	rm -rf pakext $(OBJS) *~ Debug Release debug release *.aps .*.swp

