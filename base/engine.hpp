#ifndef ENGINE_HPP
#define ENGINE_HPP 1

#include <vector>

class GameState;

class GameEngine {
public:

	void init(const char* title, int width, int height, int argc, char** args);
	void cleanup();

	void changeState(GameState* state);
	void pushState(GameState* state);
	void popState();

	void handleEvents();
	void update();
	void draw();

	bool running() { return _running; }
	void quit() { _running = false; }

private:
	// the stack of states
	std::vector<GameState*> _states;

	bool _running;
};

#endif	// ENGINE_HPP
