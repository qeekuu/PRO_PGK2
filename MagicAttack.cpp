#include "MagicAttack.h"
#include <stdexcept>
#include <cmath>

MagicAttack::MagicAttack()
{
    if (!texture.loadFromFile("magic_attack.png"))
    {
        throw std::runtime_error("Nie udalo sie wczytac tekstury ataku");
    }
    texture.setSmooth(true);
    maxFrames = texture.getSize().x / 64;
    sprite.setTexture(texture);
    sprite.setTextureRect({ {0,0},{64,64} });
    sprite.setOrigin({ 32.f,32.f });
}

void MagicAttack::start(sf::Vector2i& mousePos, sf::Vector2f& playerPos)
{
    sprite.setPosition(playerPos);
    active = true;
    lifeClock.restart();
    sf::Vector2f dir{static_cast<float>(mousePos.x) - playerPos.x,static_cast<float>(mousePos.y) - playerPos.y};
    float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
    if (length != 0.f)
    {
        dir /= length;
    }
    velocity.x = dir.x * speed.x;
    velocity.y = dir.y * speed.y;

    float angleDeg = std::atan2(dir.y, dir.x);
    sprite.setRotation(sf::radians(angleDeg));

    clock.restart();
}

void MagicAttack::update(sf::Vector2i& mousePos, sf::Vector2f& playerPos)
{
    if (!active)
    {
        return;
    }
    sprite.move(velocity);
    
    if (clock.getElapsedTime().asSeconds() >= switchTime)
    {
        currentFrame--;
        if (currentFrame == -1)
        {
            currentFrame = 1;
        }

        clock.restart();
        sprite.setTextureRect({ { static_cast<int>(currentFrame) * 64, 0 }, {64,64} });
    }
}

void MagicAttack::draw(sf::RenderWindow& window)
{
    if (active)
    {
        window.draw(sprite);
    }
}

void MagicAttack::setActive()
{
    this->active = false;
}

bool MagicAttack::isExpired(float maxLifeTime)
{
    return lifeClock.getElapsedTime().asSeconds() >= maxLifeTime;
}

sf::FloatRect MagicAttack::getBoundingBox()
{
    return this->sprite.getGlobalBounds();
}

float MagicAttack::getDamage()
{
    return this->damage;
}

void MagicAttack::setDamage(float value)
{
	this->damage = value;
}

