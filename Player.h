#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>
#include "MagicAttack.h"

enum class DIR : int
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Player
{
public:
    Player(sf::Vector2u windowSize);
    void draw(sf::RenderWindow& window);
    void move(sf::Vector2f value);
    void update(DIR newDir, bool playerIsMoving);
    DIR getPlayerDirection();
    float getSpeed();
    sf::Vector2f getPosition();
    sf::FloatRect getBoundingBox();
    void attackUpdate(sf::Vector2i mousePos, sf::Vector2f playerPos);
    void attackStart(sf::Vector2i mousePos, sf::Vector2f playerPos);
    void attackDraw(sf::RenderWindow& window);
    std::vector<std::unique_ptr<MagicAttack>>& magicAttackVector();
    int getHp();
    void setHp(unsigned int damage);
    sf::Sprite getSprite();
private:
    int             hp{ 100 };
    sf::Texture     texture;
    sf::Sprite      sprite{ texture };
    DIR             direction{ DIR::DOWN };
    unsigned int    currentFrame{ 0 };
    sf::Clock       animationClock;
    float           switchTime{ 0.07f };
    unsigned int    maxFrames{ 0 };
    float           speed = { 4.f };
    std::vector<std::unique_ptr<MagicAttack>> MagickAttacks;
    float           attackLifeTime{ 2.f };
};