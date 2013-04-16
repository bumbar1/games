#include "console.hpp"

//#include "../base/engine.hpp"

#include <fstream>

#include <mmx/util/parse.hpp>
#include <mmx/util/string.hpp>

Console::Console()
	: _opened(false)
{
	_settings = new std::map<std::string, int>;
	_buffer = new std::string;
	_font = new sf::Font;
	_history = new std::deque<std::string>;
}

Console::~Console() {
	delete _settings;
	delete _buffer;
	delete _font;
	delete _history;
}

void Console::init() {
	(*_settings)["max_fps"] = 60;
	(*_settings)["show_fps"] = 1;
	(*_settings)["time_scale"] = 100;
	(*_settings)["time_scale_normal"] = 100;
	(*_settings)["vsync"] = 0;
	(*_settings)["debug"] = 0;
	(*_settings)["quit"] = 0;
	(*_settings)["exit"] = 0;
	(*_settings)["history_size"] = 5;
	
	_lastCommand = 0;

	_font->loadFromFile("data/georgia.ttf");
}

int Console::get(const std::string& key) {
	if (_settings->count(key))
		return (*_settings)[key];
	throw "Console.get : invalid key";
}

void Console::set(const std::string& key, int value) {
	if (_settings->count(key) == 0)
		return;
	(*_settings)[key] = value;
}

void Console::log(const std::string& message) {
	std::ofstream file("console.log", std::ios::app);

	file << message << '\n';

	file.flush();
}

void Console::open() {
	_opened = true;
}

void Console::close() {
	_opened = false;
}

bool Console::is_open() const {
	return _opened;
}

/*
 * handled keys: a-z, 0-9, _ (underscore), (space), (enter), (backspace)
 */
void Console::handleEvents(GameEngine* game) {
	sf::Event event;
	while (game->screen->pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			game->quit();
			
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code >= sf::Keyboard::A && event.key.code <= sf::Keyboard::Z)
				*_buffer += event.key.code + 'a';
			
			if (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num9)
				*_buffer += event.key.code + '0' - 26; // SFML thingy (A-Z first 26 enums)
				
			if (event.key.code == sf::Keyboard::Space)
				*_buffer += ' ';
				
			if (event.key.code == sf::Keyboard::Dash && event.key.shift)
				*_buffer += '_';

			if (event.key.code == sf::Keyboard::Return) {
				*_buffer = mmx::trim(*_buffer);
				
				// empty string, no need for any checking
				if (_buffer->empty())
					continue;

				if (*_buffer == "quit" || *_buffer == "exit")
					game->quit();
			
				auto space = _buffer->find(' ');
				
				std::string cmd = _buffer->substr(0, space);
				
				int value = mmx::parse<int>(_buffer->substr(space + 1, _buffer->size()));

				set(cmd, value);
				
				_history->push_back(*_buffer);
				
				while (_history->size() > static_cast<unsigned>(get("history_size")))
					_history->pop_front();

				if (*_buffer == "max_fps")
					game->screen->setFramerateLimit(get("max_fps"));
					
				if (*_buffer == "vsync")
					game->screen->setVerticalSyncEnabled(get("vsync"));
				
				_buffer->clear();
			}
			
			if (event.key.code == sf::Keyboard::BackSpace)
				if (_buffer->size())
					_buffer->erase(_buffer->size() - 1, 1);
				
			if (event.key.code == sf::Keyboard::Escape ||
			    event.key.code == sf::Keyboard::Tilde) {
				_buffer->clear();
				close();
			}
			
			if (event.key.code == sf::Keyboard::Up) {
				if (--_lastCommand < 0)
					_lastCommand = 0;				
				*_buffer = _history->at(_history->size() - _lastCommand - 1);
			}

			if (event.key.code == sf::Keyboard::Down) {
				++_lastCommand %= _history->size();				
				*_buffer = _history->at(_lastCommand);
			}
		}
	}
}

void Console::render(GameEngine* game) {
	//game->screen->clear(sf::Color::Blue);
	
	// RenderTexture
	sf::RectangleShape box;
	box.setPosition(0, 0);
	box.setSize(sf::Vector2f(game->screen->getSize().x, 20));
	box.setFillColor(sf::Color(80, 80, 80));
	
	sf::Text text;
	text.setFont(*_font);
	text.setColor(sf::Color::White);
	text.setCharacterSize(15);
	text.setPosition(0, 0);
	text.setString(std::string("> ") + *_buffer);
	
	game->screen->draw(box);
	game->screen->draw(text);
	
	box.setPosition(0, 20);
	box.setSize(sf::Vector2f(game->screen->getSize().x, _history->size() * 16));
	box.setFillColor(sf::Color(120, 120, 120));
	
	game->screen->draw(box);
	
	for (unsigned i = 0; i < _history->size(); ++i) {
		text.setString(_history->at(i));
		text.setPosition(0, 20 + 15 * i);
		
		game->screen->draw(text);
	}
	
	game->screen->display();
}
