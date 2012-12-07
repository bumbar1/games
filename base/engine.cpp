#include "engine.hpp"
#include "state.hpp"

void GameEngine::init(const char* title, int width, int height, int argc, char** args ) {
	_running = true;
}

void GameEngine::cleanup() {
	// cleanup the all states
	while ( !_states.empty() ) {
		_states.back()->cleanup();
		_states.pop_back();
	}
}

void GameEngine::changeState(GameState* state) {
	// cleanup the current state
	if ( !_states.empty() ) {
		_states.back()->cleanup();
		_states.pop_back();
	}

	// store and init the new state
	_states.push_back(state);
	_states.back()->init();
}

void GameEngine::pushState(GameState* state) {
	// pause current state
	if ( !_states.empty() ) {
		_states.back()->pause();
	}

	// store and init the new state
	_states.push_back(state);
	_states.back()->init();
}

void GameEngine::popState() {
	// cleanup the current state
	if ( !_states.empty() ) {
		_states.back()->cleanup();
		_states.pop_back();
	}

	// resume previous state
	if ( !_states.empty() ) {
		_states.back()->resume();
	}
}


void GameEngine::handleEvents() {
	// let the state handle events
	_states.back()->handleEvents(this);
}

void GameEngine::update() {
	// let the state update the game
	_states.back()->update(this);
}

void GameEngine::draw() {
	// let the state draw the screen
	_states.back()->draw(this);
}
