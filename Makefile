ifeq ($(shell uname -s), Darwin)
LDFLAGS=-L/usr/local/Cellar/glfw3/3.2.1/lib -framework OpenGL -lglfw3 -lGLEW -lm
else
LDFLAGS=-lX11 -lGL -lGLU -lglut -lglfw -lGLEW -lm
endif
CC = g++
CFLAGS=-g -Wall -std=c++11 -I/usr/local/Cellar/glfw3/3.2.1/include

TARGET = test
SRC = $(TARGET).cc

LIB =  fin.o
all: test

test: $(SRC) $(LIB)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC) $(LIB)

#gl_replacement.o: gl_replacement.cc gl_replacement.h
#	$(CC) $(CFLAGS) -c gl_replacement.cc

fin.o: fin.cc fin.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c fin.cc

clean:
	rm $(TARGET) $(LIB)
