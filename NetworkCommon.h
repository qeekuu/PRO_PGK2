
#pragma once
#include <SFML/Network.hpp>


enum class PacketType : uint8_t {
    StateUpdate = 0,   
    SpawnEnemy = 1   
};

enum class EnemyType : uint8_t {
    Fly = 0,
    Bone = 1,
};

inline sf::Packet& operator<<(sf::Packet& p, PacketType t) 
{
    return p << static_cast<uint8_t>(t);
}
inline sf::Packet& operator>>(sf::Packet& p, PacketType& t) 
{
    uint8_t tmp;
    p >> tmp;
    t = static_cast<PacketType>(tmp);
    return p;
}

inline sf::Packet& operator<<(sf::Packet& p, EnemyType t) 
{
    return p << static_cast<uint8_t>(t);
}
inline sf::Packet& operator>>(sf::Packet& p, EnemyType& t) 
{
    uint8_t tmp; p >> tmp;
    t = static_cast<EnemyType>(tmp);
    return p;
}