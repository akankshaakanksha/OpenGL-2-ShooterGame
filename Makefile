CC = g++
CFLAGS = -Wall -w
PROG = code

SRCS = assign.cpp
LIBS = -lGL -lGLU -lGLEW -lglut 

all: $(PROG)

$(PROG):	$(SRCS)
	$(CC) $(CFLAGS) -o $(PROG) $(SRCS) $(LIBS)

clean:
	rm -f $(PROG)