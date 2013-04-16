#include "snake.hpp"

#include "../base/engine.hpp"
#include "console.hpp"

#include <mmx/util.hpp>

sf::Color colors[] = {
	sf::Color::White,
	sf::Color::Red,
	sf::Color::Green,
	sf::Color::Blue,
	sf::Color::Cyan,
	sf::Color::Magenta,
	sf::Color::Yellow
};

Snake::Snake()
	: _speed(1)
	, _size(20)
{
	for (unsigned i = 0; i < 5; ++i) {
		_body.push_back(sf::RectangleShape(sf::Vector2f(_size, _size)));
		_body.back().setFillColor(colors[mmx::ranged_rand(0, 6)]);
		_body.back().setPosition(sf::Vector2f(200 - i * _size, 100));
	}

	//_body.push_back(sf::RectangleShape(sf::Vector2f(_size, _size)));
	_body.at(0).setFillColor(sf::Color::Red);
	_direction = {1, 0};
}

/*virtual*/ void Snake::draw(sf::RenderTarget& window, sf::RenderStates states) const {
	for (auto b : _body)
		window.draw(b);
}

void Snake::handleInput(const sf::Event& event) {
	if (event.key.code == sf::Keyboard::W)
		if (_direction != sf::Vector2f(0, 1))
			_direction = sf::Vector2f(0, -1);
		
	if (event.key.code == sf::Keyboard::A)
		if (_direction != sf::Vector2f(1, 0))
			_direction = sf::Vector2f(-1, 0);
		
	if (event.key.code == sf::Keyboard::S)
		if (_direction != sf::Vector2f(0, -1))
			_direction = sf::Vector2f(0, 1);
		
	if (event.key.code == sf::Keyboard::D)
		if (_direction != sf::Vector2f(-1, 0))
			_direction = sf::Vector2f(1, 0);
		
	if (event.key.code == sf::Keyboard::Space)
		_direction = sf::Vector2f(0, 0);
		
	if (event.key.code == sf::Keyboard::Add)
		++_speed;
		
	if (event.key.code == sf::Keyboard::Subtract)
		--_speed;
		
	if (event.key.code == sf::Keyboard::Q) {
		_body.push_back(sf::RectangleShape(sf::Vector2f(_size, _size)));
		_body.back().setFillColor(colors[mmx::ranged_rand(0, 6)]);
		_body.back().setPosition(0, 0);
	}
	
	if (event.key.code == sf::Keyboard::E) {
		if (_body.size() > 1)
			_body.pop_back();
	}
}

void Snake::update(GameEngine* game, const int elapsed) {
	float scale = static_cast<float>(game->console->get("time_scale")) /
	              static_cast<float>(game->console->get("time_scale_normal"));

	float dt = static_cast<float>(elapsed) / 1000.f;

	// move head
	_body.at(0).move(_direction.x * _speed * dt * scale,
	                 _direction.y * _speed * dt * scale);
	
	// move body parts
	for (unsigned i = _body.size() - 1; i > 0; --i)
		_body.at(i).setPosition(_body.at(i - 1).getPosition().x - _size * _direction.x,
		                        _body.at(i - 1).getPosition().y - _size * _direction.y);
		
	if (_body.at(0).getPosition().x + _size > game->screen->getSize().x)
		_body.at(0).setPosition(0, _body.at(0).getPosition().y);
		
	if (_body.at(0).getPosition().x < 0)
		_body.at(0).setPosition(game->screen->getSize().x - _size, _body.at(0).getPosition().y);
		
	if (_body.at(0).getPosition().y + _size > game->screen->getSize().y)
		_body.at(0).setPosition(_body.at(0).getPosition().x, 0);
		
	if (_body.at(0).getPosition().y < 0)
		_body.at(0).setPosition(_body.at(0).getPosition().x, game->screen->getSize().y - _size);
		
	//sf::sleep(sf::milliseconds(100));
}
