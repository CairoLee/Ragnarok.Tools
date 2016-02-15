CXX = g++
CC = gcc
CFLAGS = -O2
LIBS = -lz -lGL -lGLU
SUBDIR =
OBJS = main.o

all:

clean:
	for dir in $(SUBDIR); do \
	  $(MAKE) -C $$dir clean; \
	done
	rm -rf *.o check $(OBJS) Debug Release debug release *.aps .*.swp
