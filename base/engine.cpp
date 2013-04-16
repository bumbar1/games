#include "console.hpp"
#include "engine.hpp"
#include "state.hpp"

void GameEngine::init(const char* title, int width, int height) {
	//console = new GameConsole;
	//console.init();

	//screen = new sf::RenderWindow(sf::VideoMode(width, height), title);
	screen.create(sf::VideoMode(width, height), title);
	//screen.setFramerateLimit(console.get("max_fps"));
	//screen.setVerticalSyncEnabled(console.get("vsync"));
	screen.setFramerateLimit(60);
	screen.setVerticalSyncEnabled(true);

	elapsed = 0;
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
	_states.back()->handleEvents(this);		// let the state handle events
}

void GameEngine::update() {
	_states.back().update(this);	       // let the state update the game
}

void GameEngine::render() {
	_states.back()->render(this);			// let the state render the screen
}
