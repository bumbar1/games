CC = g++

# -Wl,-subsystem,windows gets rid of the console window
FLAGS = -Wall -Wl,-subsystem,windows -s

LIBS = -lOpenGL32 -lglu32 -lfreeGLUT -lmmx

all:
	$(CC) main.cpp $(FLAGS) $(LIBS) -o snake.exe