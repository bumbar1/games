#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <string>
#include <map>
#include <deque>

#include "../base/engine.hpp"

class Console {
public:
	Console();
	~Console();
	
	int get(const std::string& key);

	void set(const std::string& key, int value);
	void log(const std::string& message);
	void init();
	void handleEvents(GameEngine* game);
	void render(GameEngine* game);
	void open();
	void close();
	
	bool is_open() const;

private:
	bool                        _opened;
	std::map<std::string, int>* _settings;
	
	std::string* _buffer;
	sf::Font*    _font;
	std::deque<std::string>*    _history;
	int                         _lastCommand;
};

#endif
