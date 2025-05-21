#pragma once
#include "Items.h"
#include<SFML/Graphics.hpp>
class EmptyItemSlot : public Items
{
public:
	EmptyItemSlot();
	sf::FloatRect getBoundingBox() override;
	void draw(sf::RenderWindow& window) override;
	sf::Sprite &getSprite() override;
private:
	static sf::Texture	EmptyItemSlotTexture;
	sf::Sprite			EmptyItemSlotSprite{ EmptyItemSlotTexture };

};

