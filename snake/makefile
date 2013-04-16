CC = g++

LIBS = -lsfml-system -lsfml-graphics -lsfml-window
FLAGS = -Wall -s -O3

OBJS = main.o playstate.o menustate.o state.o engine.o snake.o console.o

snake.exe: $(OBJS)
	$(CC) $(OBJS) $(LIBS) $(FLAGS) -o snake.exe 

engine.o: ../base/engine.cpp state.o console.o
	$(CC) -c ../base/engine.cpp -o engine.o $(LIBS) $(FLAGS)

state.o: ../base/state.cpp engine.o
	$(CC) -c ../base/state.cpp -o state.o $(FLAGS)

menustate.o: menustate.cpp engine.o state.o
	$(CC) -c menustate.cpp -o menustate.o $(LIBS) $(FLAGS)

playstate.o: playstate.cpp state.o menustate.o snake.o
	$(CC) -c playstate.cpp -o playstate.o $(LIBS) $(FLAGS)

main.o: main.cpp playstate.o menustate.o engine.o
	$(CC) -c main.cpp -o main.o $(FLAGS) -lmmx

console.o: console.cpp
	$(CC) -c console.cpp -o console.o $(FLAGS) -lmmx

snake.o: snake.cpp engine.o console.o
	$(CC) -c snake.cpp -o snake.o $(LIBS) $(FLAGS) -lmmx

clean:
	del *.o
	
opengl_snake:
# -Wl,-subsystem,windows gets rid of the console window
	$(CC) opengl_snake.cpp -Wall -Wl,-subsystem,windows -s -lOpenGL32 -lglu32 -lfreeGLUT -lmmx -o opengl_snake.exe
