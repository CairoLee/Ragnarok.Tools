CXX = g++
CC = gcc
CFLAGS = -O2 `pkg-config gtk+-2.0 --cflags` `pkg-config gtkglext-1.0 --cflags` `pkg-config libglade-2.0 --cflags`
LIBS = `pkg-config gtk+-2.0 --libs` `pkg-config gtkglext-1.0 --libs` `pkg-config libglade-2.0 --libs` -lz -lGL -lGLU
SUBDIR =
OBJS = about.o \
	act_copy.o \
	mbutton_assign.o \
	frames.o \
	table.o \
	buttons.o \
	scroll.o \
	checkbox.o \
	frame_edit.o \
	combobox.o \
	radiobutton.o \
	entry.o \
	mod_sound.o

all : $(OBJS)

.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) *~ .*.swp
