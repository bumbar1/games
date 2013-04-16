#ifndef PLAYSTATE_HPP
#define PLAYSTATE_HPP

#include "../base/state.hpp"

#include "snake.hpp"

class PlayState : public GameState {
public:
	void init();
	void cleanup();

	void pause();
	void resume();

	void handleEvents(GameEngine* game);
	void update(GameEngine* game, const int elapsed);
	void render(GameEngine* game);

	static PlayState* instance() { return &_instance; }

private:	
	static PlayState _instance;
	Snake* _snake;
};

#endif