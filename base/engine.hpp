#ifndef ENGINE_HPP
#define ENGINE_HPP 1

#include <SFML/Graphics.hpp>

#include <vector>

//class GameConsole;
class GameState;

class GameEngine {
public:
	void init(const char* title, int width, int height);
	void cleanup();

	void changeState(GameState* state);
	void pushState(GameState* state);
	void popState();

	void handleEvents();
	void update();
	void render();

	bool running() { return _running; }
	void quit() { _running = false; }

	//void openConsole() { pushState(new GameConsole); }
	//void closeConsole() { popState(); }
	
	sf::RenderWindow  screen;
	//GameConsole       console;
	double            elapsed;

private:
	// the stack of states
	std::vector<GameState*> _states;
	bool                    _running;
};

#endif	// ENGINE_HPP
