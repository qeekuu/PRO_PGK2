#include "Player.h"
#include <iostream>
#include <algorithm>

Player::Player(sf::Vector2u windowSize)
{
    if (!texture.loadFromFile("PLAYER_SPRITE.png"))
    {
        throw std::runtime_error("Nie udalo sie wczytac tekstury");
    }
    texture.setSmooth(true);
    maxFrames = texture.getSize().x / 32;
    sprite.setTexture(texture);
    sprite.setTextureRect({ {0, 0}, {32, 32} });
    sprite.setOrigin({ 16.f, 16.f });
    sprite.setPosition({ windowSize.x * 0.5f,windowSize.y * 0.5f });
    animationClock.restart();
}

void Player::draw(sf::RenderWindow& window)
{
    window.draw(sprite);
}

void Player::move(sf::Vector2f value)
{
    sprite.move(value);
}

void Player::update(DIR newDir, bool playerIsMoving)
{
    if (newDir != direction)
    {
        direction = newDir;
        currentFrame = 0;
        animationClock.restart();
    }

    if (playerIsMoving)
    {
        if (animationClock.getElapsedTime().asSeconds() >= switchTime)
        {
            currentFrame = (currentFrame + 1) % maxFrames;
            animationClock.restart();
        }
    }
    else
    {
        currentFrame = 0;
        animationClock.restart();
    }

    int y = 0;
    switch (direction)
    {
    case DIR::DOWN:
        y = 0;
        break;
    case DIR::LEFT:
        y = 64;
        break;
    case DIR::RIGHT:
        y = 96;
        break;
    case DIR::UP:
        y = 32;
        break;
    }
    sprite.setTextureRect({ {static_cast<int>(currentFrame) * 32, y}, {32, 32} });
}

void Player::attackUpdate(sf::Vector2i mousePos, sf::Vector2f playerPos)
{
    for (int i = 0;i < MagickAttacks.size();i++)
    {
        MagickAttacks[i]->update(mousePos, playerPos);
    }

    MagickAttacks.erase(std::remove_if(MagickAttacks.begin(),MagickAttacks.end(),[this](const std::unique_ptr<MagicAttack>& atk) {return atk->isExpired(attackLifeTime);}),MagickAttacks.end());
}

void Player::attackStart(sf::Vector2i mousePos,sf::Vector2f playerPos)
{
    std::unique_ptr<MagicAttack> attack = std::make_unique<MagicAttack>();
    attack->start(mousePos, playerPos);
    MagickAttacks.push_back(std::move(attack));
}

void Player::attackDraw(sf::RenderWindow& window)
{
    for (int i = 0;i < MagickAttacks.size();i++)
    {
        MagickAttacks[i]->draw(window);
    }
}

float Player::getSpeed()
{
    return this->speed;
}

sf::Vector2f Player::getPosition()
{
    return this->sprite.getPosition();
}

sf::FloatRect Player::getBoundingBox()
{
    return this->sprite.getGlobalBounds();
}

DIR Player::getPlayerDirection()
{
    return this->direction;
}

std::vector<std::unique_ptr<MagicAttack>>& Player::magicAttackVector()
{
    return this->MagickAttacks;
}

void Player::setHp(unsigned int damage)
{
    this->hp = hp - damage;
}

int Player::getHp()
{
    return this->hp;
}

sf::Sprite Player::getSprite()
{
    return this->sprite;
}
