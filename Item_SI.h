#pragma once
#include "Items.h"
class Item_SI : public Items
{
public:
	Item_SI();
	sf::FloatRect getBoundingBox() override;
	void draw(sf::RenderWindow& window) override;
	sf::Sprite& getSprite() override;
	EquipmentType getID() override;
	float getDamage() override;
private:
	static sf::Texture	Item_SITexture;
	sf::Sprite			Item_SISprite{ Item_SITexture };
	const float			damage = 2;
	EquipmentType		ID = EquipmentType::WEAPON;
};

