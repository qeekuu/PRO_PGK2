#pragma once
#include <SFML/Graphics.hpp>

class MagicAttack
{
public:
    MagicAttack();
    void start(sf::Vector2i& mousePos, sf::Vector2f& playerPos);
    void update(sf::Vector2i& mousePos, sf::Vector2f& playerPos);
    void draw(sf::RenderWindow& window);
    void setActive();
    bool isExpired(float maxLifeTime);
    sf::FloatRect getBoundingBox();
    unsigned int getDamage();
	void setDamage(float value);
private:
    unsigned int    damage{ 2 };
    sf::Texture     texture;
    sf::Sprite      sprite{ texture };
    bool            active{ false };
    sf::Vector2f    speed{ {4.f},{4.f} };
    sf::Vector2f    velocity;
    unsigned int    currentFrame{ 4 };
    sf::Clock       clock;
    float           switchTime{ 0.07f };
    unsigned int    maxFrames{ 0 };
    sf::Clock       lifeClock;
};
