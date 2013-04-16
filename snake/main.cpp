#include <SFML/System.hpp>

#include "../base/engine.hpp"

#include "playstate.hpp"
#include "menustate.hpp"

// http://files.pujs.net/all/data_tileset.png
// http://files.pujs.net/all/data_tileset2.png
// http://files.pujs.net/all/data_tileset2_hicontrast.png

#include <iostream>
#include <algorithm>

int main() {

	GameEngine game;
	
	game.init("Snake", 800, 600);
	
	game.pushState(PlayState::instance());
	game.pushState(MenuState::instance());
	
	sf::Clock clock;

	unsigned frame = 0;
	unsigned constexpr size = 100;
	unsigned measures[size]{};
	
	sf::Time old_time = clock.restart();
	
	while (game.running()) {
		//sf::Time time = clock.restart();
		sf::Time time = old_time;
		
		game.handleEvents();	
		game.update(time.asMilliseconds());
		game.render();

		measures[frame++ % size] = time.asMicroseconds();
		
		if (frame % 25 == 0) {
			auto avg = std::accumulate(std::begin(measures), std::end(measures), 0) / size;
			auto min = *std::min_element(std::begin(measures), std::end(measures)) / 1000;
			auto max = *std::max_element(std::begin(measures), std::end(measures)) / 1000;
			
			std::cout << "\r" << "avg: " << avg / 1000 << " ms (" << avg << " us), ";
			std::cout << "min: " << min << " ms, ";
			std::cout << "max: " << max << " ms, frame time: " << (clock.getElapsedTime() - time).asMicroseconds();
		}
		
		old_time = clock.restart();
	}
    
	game.cleanup();
	
	return 0;
}
