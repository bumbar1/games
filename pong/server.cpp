#include <SFML/Network.hpp>

#include <mmx/util.hpp>
#include <mmx/vector2.hpp>

#include <iostream>
#include <string>
#include <map>

#include "shared.hpp"

struct Ball {
    mmx::vector2f position;
    mmx::vector2f velocity;
    unsigned      bounce_count;
    Size          size;
    unsigned      radius;

    Ball()
        : position{0, 0}
        , velocity{0, 0}
        , bounce_count(1)
        , radius(BALL_RADIUS)
    {
    }

    void update(Score& score) {
        //position += velocity * static_cast<float>(bounce_count) / 3.f;
        position += velocity;

        if (position.y <= 0 || position.y + radius >= size.h)
            velocity.y *= -1;

        if (position.x < -100) {
            score.right++;
            respawn();
        } else if (position.x > size.w + 100) {
            score.left++;
            respawn();
        }

        //std::cout << "ball position: " << position.x << "," << position.y << "\n";
    }

    void onCollision(sf::Vector2i& player) {
        if (checkCollision(player)) {
            resolveCollision(player);
        }
    }

    bool checkCollision(sf::Vector2i& player) {
        // paddle size = 20x140
        //  left side          or right side
        if (position.x <= radius || position.x + radius >= size.w - radius) {
            //  top side              and bottom side
            if (position.y >= player.y && position.y <= player.y + 140) {
                bounce_count++;
                return true;
            }
        }
        return false;
    }

    void resolveCollision(sf::Vector2i& player) {
        velocity.x *= -1;
        //velocity.y = mmx::choice(-5, -3, 0, 3, 5);
    }

    void respawn() {
        std::cout << "last position(" << position.x << "," << position.y <<")\n";
        position = {size.w / 2 - radius / 2,
                    size.h / 2 - radius / 2};

        velocity = {mmx::choice(-6, -4, 4, 6),
                    mmx::choice(-2, 2)};

        bounce_count = 1;
    }
};

struct Player {
    sf::TcpSocket socket;
    sf::Vector2i  position;
    std::string   name;
};

class Room {
public:
    Room() {}

    void init() {
        id = 0;
        score.reset();

        ball.size.w = SCREEN_WIDTH;
        ball.size.h = SCREEN_HEIGHT;
        ball.respawn();

        running = true;
    }

    void wait_for_players(sf::TcpListener& listener) {
        sf::Packet packet;

        listener.accept(players[0].socket);

        packet << "!PLAYER 1";

        players[0].socket.send(packet);

        std::cout << "Player 1 connected\n";

        packet.clear();

        listener.accept(players[1].socket);

        packet << "!PLAYER 2";

        players[1].socket.send(packet);

        std::cout << "Player 1 connected\n";
    }

    void send_start_signal() {
        sf::Packet packet;

        packet << "!START";

        players[0].socket.send(packet);
        players[1].socket.send(packet);
    }

    void start(sf::TcpListener& listener) {
        wait_for_players(listener);
        send_start_signal();

        while (running) {

            ball.update(score);

            //sf::Packet packet = ball.pack_position();
            sf::Packet packet = pack<int>(ball.position);

            // update players with ball position
            players[0].socket.send(packet);
            players[1].socket.send(packet);

            update_player_positions();

            ball.onCollision(players[0].position);
            ball.onCollision(players[1].position);

            update_players_with_score();

            check_player_status();

            //sf::sleep(sf::milliseconds(500));
        }
    }

    void update_player_positions() {
        sf::Packet packet;

        // get player 1 position and update player 2 with it
        players[0].socket.receive(packet);
        players[1].socket.send(packet);

        // unpack to check for collisions
        packet >> players[0].position.x >> players[0].position.y;

        packet.clear();

        // get player 2 position and update player 1 with it
        players[1].socket.receive(packet);
        players[0].socket.send(packet);

        // unpack to check for collisions
        packet >> players[1].position.x >> players[1].position.y;
    }

    void update_players_with_score() {
        sf::Packet packet;

        packet << score;

        players[0].socket.send(packet);
        players[1].socket.send(packet);
    }

    void check_player_status() {
        sf::Packet packet;
        std::string status[4] = {"", "", "!", "!"};

        players[0].socket.receive(packet);
        packet >> status[0];

        packet.clear();

        players[1].socket.receive(packet);
        packet >> status[1];

        if (status[0] == "!QUIT") {
            std::cout << "Player 1 disconnected.\n";
            status[2] = "!PLAYER DISCONNECTED";
            running = false;
        }

        if (status[1] == "!QUIT") {
            std::cout << "Player 2 disconnected.\n";
            status[3] = "!PLAYER DISCONNECTED";
            running = false;
        }

        packet << status[3];
        players[0].socket.send(packet);

        packet.clear();

        packet << status[2];
        players[1].socket.send(packet);
    }

private:
    Player   players[2];
    //std::vector<sf::TcpSocket*> spectators;
    Ball     ball;
    Score    score;
    unsigned id;
    bool     running;
};

class Server {
public:
    Server() {}

    void init() {
        listener.listen(PORT);

        room.init();
    }

    void start() {
        std::cout << "Server room started\n";

        room.start(listener);
    }

private:
    //std::map<unsigned, Room> rooms;
    Room room;
    static constexpr unsigned PORT = 50001;
    sf::TcpListener  listener;
    bool running;
};

int main() {

    Server server;

    server.init();

    server.start();

    return 0;
}
