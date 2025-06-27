#include "Player.h"
#include "Items.h"
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
    EQ = Inventory();
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

void Player::attackStart(sf::Vector2f mousePos,sf::Vector2f playerPos)
{
    std::unique_ptr<MagicAttack> attack = std::make_unique<MagicAttack>();
    attack->start(mousePos, playerPos);
    attack->setDamage(this->playerDamage);
    MagickAttacks.push_back(std::move(attack));
}

void Player::attackDraw(sf::RenderWindow& window)
{
    for (int i = 0;i < MagickAttacks.size();i++)
    {
        MagickAttacks[i]->draw(window);
    }
}

void Player::drawEQ(sf::RenderWindow& window)
{
    EQ.draw(window);
}

Inventory &Player::getInventory()
{
    return this->EQ;
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
    this->hp -= damage;
}

float Player::getHp()
{
    return this->hp;
}

sf::Sprite Player::getSprite()
{
    return this->sprite;
}


std::map<EquipmentType, std::unique_ptr<Items>> &Player::getEquipment()
{
    return this->playerEQ;
}

void Player::setDamage(float value)
{
    this->playerDamage += value;
}

void Player::setArmor(float value)
{
    this->armor += value;
}

void Player::setLevel(float value)
{
    this->level += value;
}

void Player::setTreshold(float value)
{
    this->characterLevelTreshold += value;
}

int Player::getLevel()
{
    return this->level;
}

int Player::getTreshold()
{
    return this->characterLevelTreshold;
}

float Player::getXp()
{
    return this->xp;
}

void Player::setXp(float value)
{
    this->xp += value;
}

void Player::setSpeed(float value)
{
    this->speed += value;
}

void Player::setHPUpgrade(float value)
{
    this->hp += value;
}

void Player::reset()
{
    this->armor = 1;
    this->hp = 100;
    this->xp = 0;
    this->level = 1;
    this->characterLevelTreshold = 100;
    this->playerDamage = 2.0f;
    this->speed = 4.f;
}

float                                               hp{ 100 };
int                                                 level{ 1 };
int				                                    armor{ 1 };
float                                               characterLevelTreshold{ 100 };
float                                               xp{ 0 };
float			                                    playerDamage{ 2.0f };
sf::Texture                                         texture;
sf::Sprite                                          sprite{ texture };
DIR                                                 direction{ DIR::DOWN };
unsigned int                                        currentFrame{ 0 };
sf::Clock                                           animationClock;
float                                               switchTime{ 0.07f };
unsigned int                                        maxFrames{ 0 };
float                                               speed = { 4.f };
std::vector<std::unique_ptr<MagicAttack>>           MagickAttacks;
float                                               attackLifeTime{ 2.f };
Inventory                                           EQ;
std::map<EquipmentType, std::unique_ptr<Items>>     playerEQ;