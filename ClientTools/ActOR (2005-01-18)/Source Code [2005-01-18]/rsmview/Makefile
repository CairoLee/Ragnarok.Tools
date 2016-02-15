CXX = g++
CC = gcc
CFLAGS = -O3 `pkg-config gtk+-2.0 --cflags` `pkg-config gtkglext-1.0 --cflags`
LIBS = `pkg-config gtk+-2.0 --libs` `pkg-config gtkglext-1.0 --libs` -lGL -lGLU -lgettextlib
SUBDIR =
OBJS = main.o \
	rsmview.o
RELOBJS = ../common/pak.o \
	../common/rsm.o \
	../common/vec.o \
	../actor/gtk/mbutton_assign.o

all: rsmview

rsmview: $(OBJS) $(RELOBJS)
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
	rm -rf rsmview $(OBJS) *~ Debug Release *.aps .*.swp

