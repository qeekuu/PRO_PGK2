#include "Item_SI.h"

Item_SI::Item_SI()
{
	if (Item_SITexture.getSize().x == 0)
	{
		if (!Item_SITexture.loadFromFile("SI.png"))
		{
			throw std::runtime_error("Error during loading a texture");
		}
		Item_SITexture.setSmooth(true);
	}
	this->Item_SISprite.setTexture(Item_SITexture);
	this->Item_SISprite.setTextureRect({ {0, 0}, {32, 32} });
	this->Item_SISprite.setOrigin({ 16,16 });
}

sf::FloatRect Item_SI::getBoundingBox()
{
	return this->Item_SISprite.getGlobalBounds();
}

void Item_SI::draw(sf::RenderWindow& window)
{
	window.draw(Item_SISprite);
}

sf::Sprite& Item_SI::getSprite()
{
	return this->Item_SISprite;
}

EquipmentType Item_SI::getID()
{
	return this->ID;
}

float Item_SI::getDamage()
{
	return this->damage;
}

sf::Texture Item_SI::Item_SITexture;