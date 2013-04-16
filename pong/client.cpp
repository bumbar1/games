#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <mmx/util/parse.hpp>
#include <mmx/util/string.hpp>

#include <string>
#include <iostream>

#include "shared.hpp"

class Player : public sf::Drawable {
public:
	Player(const char* host, unsigned port) {
		socket.connect(host, port);

		player.setSize(sf::Vector2f(20, 7 * 20));
		other.setSize(sf::Vector2f(20, 7 * 20));

        player.setPosition(0, SCREEN_HEIGHT / 2 - 3 * 20);
        other.setPosition(0, SCREEN_HEIGHT  / 2 - 3 * 20);

		ball.setFillColor(sf::Color::White);
		ball.setRadius(BALL_RADIUS);

        font = new sf::Font;
		if (!font->loadFromFile("arial.ttf"))
            std::cout << "Error loading font\n";

		text.setFont(*font);
        text.setCharacterSize(20);
	    text.setColor(sf::Color::Yellow);
	    text.setPosition(SCREEN_WIDTH / 2 - 50, 50);

	    quitFlag = false;
	}

    void quit() { quitFlag = true; }

	void handle_keys(const sf::Event& event) {
		if (event.key.code == sf::Keyboard::Up)
            if (player.getPosition().y > 0)
                player.move(0, -20);

		if (event.key.code == sf::Keyboard::Down)
			if (player.getPosition().y + player.getSize().y < SCREEN_HEIGHT)
                player.move(0, 20);
	}

	bool wait_for_start() {
		std::string msg;
		sf::Packet packet;

        // player id
		socket.receive(packet);

		packet >> msg;

        if (msg == "!PLAYER 1") { player_position = 0; }
        else if (msg == "!PLAYER 2") { player_position = 1; }
        else
            throw std::string("invalid player position: ") + msg;

        std::cout << "You are " << msg;

        if (player_position == 0) {
            player.setFillColor(sf::Color::Red);
            other.setFillColor(sf::Color::Blue);
            std::cout << " (red)\n";
        } else {
            player.setFillColor(sf::Color::Blue);
            other.setFillColor(sf::Color::Red);
            std::cout << " (blue)\n";
        }

        player.move(player_position * (SCREEN_WIDTH - 20), 0);
        other.move(!player_position * (SCREEN_WIDTH - 20), 0);

        packet.clear();

        // good to go?
        socket.receive(packet);
        msg.clear();
        packet >> msg;

		if (msg == "!START")
			return true;
		throw std::string("error getting start signal: ") + msg;
	}

	void update_with_server() {
        // get ball position
        // get other position
        // send player position
        sf::Packet packet;

        unsigned x = 0, y = 0;

        // ball position
        socket.receive(packet);

        packet >> x >> y;

        ball.setPosition(x, y);

        packet.clear();

        /*packet << static_cast<unsigned>(player.getPosition().x)
               << static_cast<unsigned>(player.getPosition().y);*/
        packet = pack<int>(player.getPosition());

        socket.send(packet);

        packet.clear();

        // other player position
        socket.receive(packet);

        packet >> x >> y;

        other.setPosition(x, y);

        packet.clear();

        socket.receive(packet);

        packet >> score;

        text.setString(mmx::to_string(score.left) + " : " + mmx::to_string(score.right));

        packet.clear();

        if (quitFlag)
            packet << "!QUIT";
        else
            packet << "!PLAYING";

        socket.send(packet);

        // check other player stats (still there?)
        packet.clear();

        std::string msg;

        socket.receive(packet);

        packet >> msg;

        if (msg == "!PLAYER DISCONNECTED") {
            std::cout << "Your opponent has fled the battlefield!\n";
            quit();
        }
	}

	void draw(sf::RenderTarget& target, sf::RenderStates states) const {

	    target.draw(text);

		target.draw(player);
		target.draw(other);
		target.draw(ball);
	}

private:
	sf::RectangleShape  player;
	sf::RectangleShape  other;
	sf::TcpSocket       socket;
	unsigned            player_position; // 0 or  1
	sf::CircleShape     ball;
	Score               score;
	sf::Font            *font;
	sf::Text            text;
	bool                quitFlag;
};

int main(int argc, char* argv[]) {

	if (argc != 3) {
		std::cout << "usage: " << argv[0] << " [ip] [port]\n";
		return 1;
	}

	Player player(argv[1], mmx::parse<unsigned>(argv[2]));

    try {
        player.wait_for_start();

    } catch (std::string& e) {
        std::cout << "exception: " << e << "\n";
    } catch (const char* e) {
        std::cout << "exception: " << e << "\n";
    } catch (...) {
        std::cout << "unhandled exception\n";
    }

	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Pong");
    window.setFramerateLimit(60);
    //window.setKeyRepeatEnabled(false);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
				player.quit();
			}

			if (event.type == sf::Event::KeyPressed)
				player.handle_keys(event);
		}

        player.update_with_server();

		window.clear();

		window.draw(player);

		window.display();
	}

	return 0;
}
