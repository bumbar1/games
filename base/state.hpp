#ifndef STATE_HPP
#define STATE_HPP 1

class GameEngine;

class GameState {
public:
	GameState(const GameState& ) = delete;
	GameState& operator = (const GameState& ) = delete;
	
	GameState(GameState&& ) = delete;
	GameState&& operator = (GameState&& ) = delete;
	
	virtual void init() = 0;
	virtual void cleanup() = 0;

	virtual void pause() = 0;
	virtual void resume() = 0;

	virtual void handleEvents(GameEngine* game) = 0;
	virtual void update(GameEngine* game) = 0;
	virtual void render(GameEngine* game) = 0;

	void changeState(GameEngine* game, GameState* state);

protected:
	GameState() {}
};

#endif	// STATE_HPP
