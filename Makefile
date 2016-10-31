ifeq ($(shell uname -s), Darwin)
LDFLAGS=-L/opt/ports/lib -framework OpenGL -lglfw -lGLEW -lm
else
LDFLAGS=-lX11 -lGL -lGLU —lglut -lglfw -lGLEW -lm
endif
CC = g++
CFLAGS=-g -Wall -std=c++11 -I/opt/ports/include

TARGET = mesh
SRC = $(TARGET).cc

LIB =  fin.o initshader.o
all: mesh

mesh: $(SRC) $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC) $(LIB)

fin.o: fin.cc fin.h
	$(CC) $(CFLAGS) -c fin.cc

initshader.o: initshader.c
	$(CC) $(CFLAGS) -c initshader.c

clean:
	rm $(TARGET) $(LIB)
