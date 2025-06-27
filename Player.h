#pragma once
#include <SFML/Graphics.hpp>
#include <stdexcept>
#include "MagicAttack.h"
#include <iostream>
#include <algorithm>
#include <map>
#include "EquipmentType.h"
#include "Inventory.h"
enum class DIR : int
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};
class Items;
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
    void attackStart(sf::Vector2f mousePos, sf::Vector2f playerPos);
    void attackDraw(sf::RenderWindow& window);
    std::vector<std::unique_ptr<MagicAttack>>& magicAttackVector();
    float getHp();
    void setHp(unsigned int damage);
    sf::Sprite getSprite();
    void drawEQ(sf::RenderWindow& window);
    Inventory& getInventory();
    std::map<EquipmentType, std::unique_ptr<Items>> &getEquipment();
    void setDamage(float value);
    void setArmor(float value);
    void setLevel(float value);
    void setTreshold(float value);
    void setXp(float value);
    void setSpeed(float value);
    void setHPUpgrade(float value);
    int getLevel();
    int getTreshold();
    float getXp();
    void reset();
private:
    float                                               hp{ 100 };
    int                                                 level{ 1 };
	int				                                    armor { 1 };
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
};
