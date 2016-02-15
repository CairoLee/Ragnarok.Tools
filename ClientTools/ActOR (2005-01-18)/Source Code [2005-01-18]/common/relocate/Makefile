CXX = g++
CC = gcc
CFLAGS = -O2
OBJS = progname.o \
	relocatable.o

all: $(OBJS)

.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) *~ .*.swp

