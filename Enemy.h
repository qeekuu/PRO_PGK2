#pragma once
#include <SFML/Graphics.hpp>
#include "NetworkCommon.h"
class Enemy
{
public:
	virtual void draw(sf::RenderWindow& window) = 0;
	virtual void update(sf::Vector2f playerPosition) = 0;
	virtual void move(sf::Vector2f playerPosition) = 0;
	virtual sf::FloatRect getBoundingBox() = 0;
	virtual  ~Enemy() {};
	virtual unsigned int getId() = 0;
	virtual void setID(unsigned int id) = 0;
	virtual void setHp(float damage) = 0;
	virtual float getHp() = 0;
	virtual unsigned int getDamage() = 0;
	virtual bool canDealDamage() = 0;
	virtual void notifyDamageDealt() = 0;
	virtual sf::Vector2f getPosition() = 0;
	virtual void setPosition(sf::Vector2f pos) = 0;
	virtual EnemyType getType() = 0;
};

