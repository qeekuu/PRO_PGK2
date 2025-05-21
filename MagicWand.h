#pragma once
#include "Items.h"
#include <SFML/Graphics.hpp>
#include "Player.h"
class MagicWand : public Items
{
public:
	MagicWand();
	sf::FloatRect getBoundingBox() override;
	void draw(sf::RenderWindow& window) override;
	sf::Sprite &getSprite() override;
	EquipmentType getID() override;
	float getDamage() override;
private:
	static sf::Texture	magicWandTexture;
	sf::Sprite			magicWandSprite{ magicWandTexture };
	const float			damage = 1.5f;
	EquipmentType		ID = EquipmentType::WEAPON;
	
};

