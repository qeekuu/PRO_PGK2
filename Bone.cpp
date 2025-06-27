#include "Bone.h"
#include <iostream>
#include <cmath>
Bone::Bone() : ID{ nextID++ }
{
    if (sharedTexture.getSize().x == 0)
    {
        if (!sharedTexture.loadFromFile("SKELETON.png"))
        {
            throw std::runtime_error("Error during loading a texture");
        }
        sharedTexture.setSmooth(true);
    }

    this->maxFrames = this->sharedTexture.getSize().x / 32;
    this->sprite.setTexture(this->sharedTexture);
    this->sprite.setTextureRect({ {0, 0}, {32, 61} });
    this->sprite.setOrigin({16.f, 30.5f });
    this->sprite.setPosition({ 300,300 });
    this->animationClock.restart();
}
void Bone::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

void Bone::update(sf::Vector2f playerPosition)
{
    sf::Vector2f playerPos = playerPosition;
    sf::Vector2f monsterPos = sprite.getPosition();
    if (this->animationClock.getElapsedTime().asSeconds() >= this->switchTime)
    {
        this->currentFrame = (this->currentFrame + 1) % this->maxFrames;
        this->animationClock.restart();
    }
    if (monsterPos.x > playerPos.x)
    {
        this->sprite.setTextureRect({ {static_cast<int>(this->currentFrame) * 32, 0}, {32, 61} });
    }
    else
    {
        this->sprite.setTextureRect({ {static_cast<int>(this->currentFrame) * 32, 61}, {32, 61} });
    }



}

void Bone::move(sf::Vector2f playerPosition)
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

unsigned int Bone::getId()
{
    return this->ID;
}

sf::FloatRect Bone::getBoundingBox()
{
    return this->sprite.getGlobalBounds();
}

void Bone::setHp(float damage)
{
    this->hp = hp - damage;
}

float Bone::getHp()
{
    return this->hp;
}

unsigned int Bone::getDamage()
{
    return this->damage;
}

bool Bone::canDealDamage()
{
    return damageClock.getElapsedTime().asSeconds() >= DAMAGE_COOLDOWN;
}

void Bone::notifyDamageDealt()
{
    damageClock.restart();
}

sf::Vector2f Bone::getPosition()
{
    return this->sprite.getPosition();
}

void Bone::setPosition(sf::Vector2f pos)
{
    this->sprite.setPosition(pos);
}

void Bone::setID(unsigned int id) 
{
    this->ID = id;
}

unsigned int Bone::nextID{ 1 };
sf::Texture Bone::sharedTexture;
const float Bone::DAMAGE_COOLDOWN{ 2.f };
