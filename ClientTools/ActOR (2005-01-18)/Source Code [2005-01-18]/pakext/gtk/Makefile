CXX = g++
CC = gcc
CFLAGS = -O2 `pkg-config gtk+-2.0 --cflags` `pkg-config gtkglext-1.0 --cflags`
LIBS = `pkg-config gtk+-2.0 --libs` `pkg-config gtkglext-1.0 --libs` -lz -lGL -lGLU
SUBDIR =
OBJS = chrview_gtk.o \
	easyext_gtk.o \
	mp98_gtk.o

all: $(OBJS)

.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) *~ .*.swp
