#ifndef MENUSTATE_HPP
#define MENUSTATE_HPP

#include "../base/state.hpp"

#include <SFML/Graphics.hpp>

class MenuState : public GameState {
public:
	void init();
	void cleanup();

	void pause();
	void resume();

	void handleEvents(GameEngine* game);
	void update(GameEngine* game, const int elapsed);
	void render(GameEngine* game);

	static MenuState* instance() { return &_instance; }

private:	
	static MenuState _instance;
	sf::Font*        _font;
};

#endif
