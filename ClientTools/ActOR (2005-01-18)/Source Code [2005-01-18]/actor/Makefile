CXX = g++
CC = gcc
CFLAGS = -O3 `pkg-config gtk+-2.0 --cflags` `pkg-config gtkglext-1.0 --cflags` `pkg-config libglade-2.0 --cflags`
LIBS = `pkg-config gtk+-2.0 --libs` `pkg-config gtkglext-1.0 --libs` `pkg-config libglade-2.0 --libs` -lGL -lGLU -lgettextlib -lz
SUBDIR = gtk win32
OBJS = main.o \
	actor.o \
	gtk/act_copy.o \
	gtk/about.o \
	gtk/mbutton_assign.o \
	gtk/frames.o \
	gtk/table.o \
	gtk/scroll.o \
	gtk/buttons.o \
	gtk/checkbox.o \
	gtk/frame_edit.o \
	gtk/combobox.o \
	gtk/radiobutton.o \
	gtk/entry.o \
	gtk/mod_sound.o
RELOBJS = ../common/Sprite.o \
	../common/act.o \
	../common/spr.o \
	../common/vec.o \

all: actor.glade.z actor

actor: $(OBJS) $(RELOBJS)
	for dir in $(SUBDIR); do \
	  $(MAKE) -C $$dir; \
	done
	$(CXX) $(CFLAGS) -o $@ $(OBJS) \
	$(RELOBJS) $(LIBS)

actor.glade.z: actor.glade
	./glade2charZ.pl actor.glade > actor.glade.z
	
.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	for dir in $(SUBDIR); do \
	  $(MAKE) -C $$dir clean; \
	done
	rm -rf actor $(OBJS) *~ Debug Release debug release *.aps .*.swp
	rm -f *.glade.bak *.gladep *.gladep.bak
	rm -f actor.glade.z


