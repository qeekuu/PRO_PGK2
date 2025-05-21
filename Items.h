#pragma once
#include <SFML/Graphics.hpp>
#include "EquipmentType.h"
class Items
{
public:
	virtual void draw(sf::RenderWindow& window) = 0;
	virtual sf::FloatRect getBoundingBox() = 0;
	virtual sf::Sprite &getSprite() = 0;
	virtual EquipmentType getID() = 0;
	virtual float getDamage() = 0;
};

