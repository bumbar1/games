#ifndef CONSOLE_HPP
#define CONSOLE_HPP 1

#include <deque>
#include <string>
#include <map>

#include <SFML/Graphics.hpp>

class GameEngine;

class GameConsole : public GameState {
public:
	GameConsole();
	~GameConsole();

	void init() {}
	void cleanup() {}
	
	void pause() {}
	void resume() {}
	
	void handleEvents(GameEngine* game);
	void update(GameEngine* game) {}
	void render(GameEngine* game);

	int get(const std::string& key);

	bool set(const std::string& key, int value);
	//void log(const std::string& message);
	//void init();
	//void handleEvents(GameEngine* game);
	//void render(GameEngine* game);

private:
	void execute(GameEngine* game);

	int                         _lastCommand;
	std::map<std::string, int>  _settings;
	std::deque<std::string>     _history;
	std::string                 _buffer;
	sf::Font                    _font;
};

#endif    // CONSOLE_HPP
