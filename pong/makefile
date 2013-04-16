CC = g++

LIBS = -lOpenGL32 -lglu32 -lfreeGLUT -lmmx

# -Wl,-subsystem,windows gets rid of the console window
FLAGS = -Wall -s -O3 -Wl,-subsystem,windows

all:
	$(CC) $(FLAGS) $(LIBS) opengl_pong.cpp -o opengl_pong.exe
