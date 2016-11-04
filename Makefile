ifeq ($(shell uname -s), Darwin)
LDFLAGS=-L/usr/local/Cellar/glfw3/3.2.1 -framework OpenGL -lglfw3 -lGLEW -lm
else
LDFLAGS=-lX11 -lGL -lGLU -lglut -lGLEW -lglfw3 -lm 
endif
CC = g++
CFLAGS=-g -Wall -std=c++11 -I/usr/local/Cellar/glfw3/3.2.1

TARGET = mesh
SRC = $(TARGET).cc

LIB = fin.o initshader.o
all: mesh

mesh: $(SRC) $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC) $(LIB)

fin.o: fin.cc fin.h
	$(CC) $(CFLAGS) -c fin.cc

initshader.o: initshader.c
	$(CC) $(CFLAGS) -c initshader.c

clean:
	rm $(TARGET) $(LIB)
