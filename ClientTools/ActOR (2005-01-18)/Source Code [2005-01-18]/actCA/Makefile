CXX = g++
CC = gcc
CFLAGS =
LIBS =
SUBDIR =
OBJS = main.o 
RELOBJS = ../common/act.o \
	../common/getopt.o \

all: actCA

actCA: $(OBJS) $(RELOBJS)
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
	rm -rf actCA $(OBJS) *~ Debug Release debug release *.aps .*.swp

