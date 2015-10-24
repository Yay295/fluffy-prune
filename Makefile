

# GNU C/C++ compiler and linker:
LINK = g++

CFLAGS = -Wall -O
CXXFLAGS = $(CFLAGS)

# OpenGL/Mesa libraries for Linux
GL_LIBS = -lglut -lGLU -lGL -lm
# GL_LIBS =  -framework GLUT -framework OpenGL -framework Cocoa 


all:	Main

Main:	Main.o quadtree.o
	$(LINK) -o $@ $^ $(GL_LIBS)

clean:
	rm -f *.o ~* core a.out

