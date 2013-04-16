#include "playstate.hpp"
#include "menustate.hpp"

#include "console.hpp"

#include "../base/engine.hpp"

PlayState PlayState::_instance;

void PlayState::init() { _snake = new Snake; }
void PlayState::cleanup() { delete _snake; }

void PlayState::pause() {}
void PlayState::resume() {}

void PlayState::handleEvents(GameEngine* game) {
	sf::Event event;
	while (game->screen->pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			game->quit();
			
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape)
				game->pushState(MenuState::instance());
				
			if (event.key.code == sf::Keyboard::Tilde)
				game->console->open();

			_snake->handleInput(event);
		}
	}
}

void PlayState::update(GameEngine* game, const int elapsed) {
	_snake->update(game, elapsed);
}

void PlayState::render(GameEngine* game) {
	game->screen->clear(sf::Color::Black);
	
	game->screen->draw(*_snake);
	
	game->screen->display();
}
