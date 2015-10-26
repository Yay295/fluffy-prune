SOURCE = Main.cpp \
         loadBMP.cpp \
         quadtree.cpp
OBJS = $(SOURCE:.cpp=.o)

#GNU C/C++ Compiler
GCC = g++

# GNU C/C++ Linker
LINK = g++

# Compiler flags
CFLAGS = -Wall -O -std=c++11 -c
CXXFLAGS = $(CFLAGS)

# OpenGL/Mesa libraries for Linux
GL_LIBS = -lglut -lGLU -lGL -lm

.PHONY: clean

# Targets include all, clean, debug, tar

all : Main

Main: $(OBJS)
	$(LINK) -o $@ $^ $(GL_LIBS)

clean:
	rm -rf *.o *.d core Main

debug: CXXFLAGS += -DDEBUG -g
debug: Main

tar:
	tar cvf Main_Tarball.tar *.cpp *.h Makefile

help:
	@echo "	make Main  - same as make all"
	@echo "	make all   - builds the main target"
	@echo "	make       - same as make all"
	@echo "	make clean - remove .o .d core Main"
	@echo "	make debug - make all with -g and -DDEBUG"
	@echo "	make tar   - make a tarball of .cpp and .h files"
	@echo "	make help  - this message"

%.d: %.cpp
	@set -e; /usr/bin/rm -rf $@;$(GCC) -MM $< $(CXXFLAGS) > $@

