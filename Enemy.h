#pragma once
#include <SFML/Graphics.hpp>
class Enemy
{
public:
	virtual void draw(sf::RenderWindow& window) = 0;
	virtual void update(sf::Vector2f playerPosition) = 0;
	virtual void move(sf::Vector2f playerPosition) = 0;
	virtual sf::FloatRect getBoundingBox() = 0;
	virtual  ~Enemy() {};
	virtual unsigned int getId() = 0;
	virtual void setHp(unsigned int damage) = 0;
	virtual unsigned int getHp() = 0;
	virtual unsigned int getDamage() = 0;
	virtual bool canDealDamage() = 0;
	virtual void notifyDamageDealt() = 0;
	virtual sf::Vector2f getPosition() = 0;
};

