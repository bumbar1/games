#ifndef SNAKE_HPP
#define SNAKE_HPP

#include <SFML/Graphics.hpp>

#include <vector>

class GameEngine;

class Snake : public sf::Drawable {
public:
	Snake();

	virtual void draw(sf::RenderTarget& window, sf::RenderStates states) const;
	
	void handleInput(const sf::Event& event);
	void update(GameEngine* game, const int elapsed);
	
	sf::Vector2f _direction;
	
private:
	std::vector<sf::RectangleShape> _body;
	unsigned                        _speed;
	unsigned                        _size;
	
};

#endif
