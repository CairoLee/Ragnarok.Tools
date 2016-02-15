MAKE = make
CXX = g++
CC = gcc
CFLAGS = -O3
SUBDIR = relocate
OBJS = pak.o \
	act.o \
	spr.o \
	Sprite.o \
	rsm.o \
	vec.o \
	getopt.o \
	codeconv.o 

all: $(OBJS)
	for dir in $(SUBDIR); do \
	  $(MAKE) -C $$dir; \
	done

.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	for dir in $(SUBDIR); do \
	  $(MAKE) -C $$dir clean; \
	done
	rm -f $(OBJS) *~ .*.swp

