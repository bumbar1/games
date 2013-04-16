#include "menustate.hpp"

#include "../base/engine.hpp"

#include "console.hpp"

MenuState MenuState::_instance;

void MenuState::init() {	
	_font = new sf::Font;
	_font->loadFromFile("data/georgia.ttf");
}
void MenuState::cleanup() {
	delete _font;
}

void MenuState::pause() {}
void MenuState::resume() {}

void MenuState::handleEvents(GameEngine* game) {
	sf::Event event;
	while (game->screen->pollEvent(event)) {
		if (event.type == sf::Event::Closed)
			game->quit();
			
		if (event.type == sf::Event::KeyPressed) {
			if (event.key.code == sf::Keyboard::Escape || event.key.code == sf::Keyboard::Q)
				game->quit();
				
			if (event.key.code == sf::Keyboard::P)
				game->popState();

			if (event.key.code == sf::Keyboard::Tilde)
				game->console->open();
		}
	}
}

void MenuState::update(GameEngine* game, const int elapsed) {}

void MenuState::render(GameEngine* game) {
	game->screen->clear(sf::Color::Black);

	const char* texts[] = {
		"[P]LAY",
		"[Q]UIT"
	};
	
	unsigned size = sizeof(texts) / sizeof(texts[0]);

	for (unsigned i = 0; i < size; ++i) {
		sf::Text text(texts[i], *_font);
		
		text.setPosition(static_cast<float>(game->screen->getSize().x / 2 - text.getLocalBounds().width / 2),
		                 static_cast<float>(game->screen->getSize().y / 4 + i * 100));
		
		text.setColor(sf::Color::White);
		game->screen->draw(text);
	}
	
	game->screen->display();
}
