#ifndef SHARED_HPP
#define SHARED_HPP 1

#include <SFML/Network/Packet.hpp>

#include <mmx/vector2.hpp>

const unsigned SCREEN_WIDTH  = 800;
const unsigned SCREEN_HEIGHT = 600;
const unsigned BALL_RADIUS   = 10;

struct Size {
    unsigned w, h;
};

struct Score {
    unsigned left;
    unsigned right;

    void reset() {
        left = 0;
        right = 0;
    }
};

sf::Packet& operator << (sf::Packet& packet, const Score& score) {
    return packet << score.left << score.right;
}

sf::Packet& operator >> (sf::Packet& packet, Score& score) {
    return packet >> score.left >> score.right;
}

template <class T, class U>
sf::Packet pack(const mmx::vector2<U>& v) {
    sf::Packet packet;

    packet << static_cast<T>(v.x) << static_cast<T>(v.y);

    return packet;
}

template <class T, class U>
sf::Packet pack(const sf::Vector2<U>& v) {
    sf::Packet packet;

    packet << static_cast<T>(v.x) << static_cast<T>(v.y);

    return packet;
}

template <class T, class U>
void unpack(sf::Packet& packet, mmx::vector2<U>& v) {
    packet >> static_cast<T>(v.x) >> static_cast<T>(v.y);
}

template <class T, class U>
void unpack(sf::Packet& packet, sf::Vector2<U>& v) {
    packet >> static_cast<T>(v.x) >> static_cast<T>(v.y);
}

#endif
