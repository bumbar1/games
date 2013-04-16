#include "state.hpp"
#include "engine.hpp"

void GameState::changeState(GameEngine* game, GameState* state) {
	game->changeState(state);
}
