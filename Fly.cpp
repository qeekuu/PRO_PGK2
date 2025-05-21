#include "Fly.h"
#include <iostream>
#include <cmath>
Fly::Fly() : ID{nextID++}
{
    if (sharedTexture.getSize().x == 0) 
    {
        if (!sharedTexture.loadFromFile("greyfly_spritesheet.png"))
        {
            throw std::runtime_error("Error during loading a texture");
        }
        sharedTexture.setSmooth(true);
    }

    this->maxFrames = this->sharedTexture.getSize().x / 16;
    this->sprite.setTexture(this->sharedTexture);
    this->sprite.setTextureRect({ {0, 0}, {16, 16} });
    this->sprite.setOrigin({ 8.f, 8.f });
    this->sprite.setPosition({ 300,300 });
    this->animationClock.restart();
}
void Fly::draw(sf::RenderWindow& window)
{
	window.draw(sprite);
}

void Fly::update(sf::Vector2f playerPosition)
{
    sf::Vector2f playerPos = playerPosition;
    sf::Vector2f monsterPos = sprite.getPosition();
    if (this->animationClock.getElapsedTime().asSeconds() >= this->switchTime)
    {
        this->currentFrame = (this->currentFrame + 1) % this->maxFrames;
        this->animationClock.restart();
    }
    if (monsterPos.x < playerPos.x)
    {
        this->sprite.setTextureRect({ {static_cast<int>(this->currentFrame) * 16, 0}, {16, 16} });
    }
    else
    {
        this->sprite.setTextureRect({ {static_cast<int>(this->currentFrame) * 16, 16}, {16, 16} });
    }
    

    
}

void Fly::move(sf::Vector2f playerPosition)
{
    sf::Vector2f playerPos = playerPosition;
    sf::Vector2f monsterPos = sprite.getPosition();
    sf::Vector2f direction = playerPos - monsterPos;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

    if (length > 0.001f)
    {
        sf::Vector2f directionNormalized = { direction.x / length, direction.y / length };
        sprite.move(directionNormalized * speed);
    }
    else
    {

    }
}

unsigned int Fly::getId()
{
    return this->ID;
}

sf::FloatRect Fly::getBoundingBox()
{
    return this->sprite.getGlobalBounds();
}

void Fly::setHp(float damage)
{
    this->hp = hp - damage;
}

float Fly::getHp()
{
    return this->hp;
}

unsigned int Fly::getDamage()
{
    return this->damage;
}

bool Fly::canDealDamage()
{
    return damageClock.getElapsedTime().asSeconds() >= DAMAGE_COOLDOWN;
}

void Fly::notifyDamageDealt() 
{
    damageClock.restart();
}

sf::Vector2f Fly::getPosition()
{
    return this->sprite.getPosition();
}
unsigned int Fly::nextID{ 1 };
sf::Texture Fly::sharedTexture;
const float Fly::DAMAGE_COOLDOWN{ 2.f };
