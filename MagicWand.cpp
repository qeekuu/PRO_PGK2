#include "MagicWand.h"

MagicWand::MagicWand()
{
	if (magicWandTexture.getSize().x == 0)
	{
		if (!magicWandTexture.loadFromFile("Magic_Wand.png"))
		{
			throw std::runtime_error("Error during loading a texture");
		}
		magicWandTexture.setSmooth(true);
	}
	this->magicWandSprite.setTexture(magicWandTexture);
	this->magicWandSprite.setTextureRect({ {0, 0}, {32, 32} });
	this->magicWandSprite.setOrigin({16,16});
}

sf::FloatRect MagicWand::getBoundingBox()
{
	return this->magicWandSprite.getGlobalBounds();
}

void MagicWand::draw(sf::RenderWindow& window)
{
	window.draw(magicWandSprite);
}

sf::Sprite &MagicWand::getSprite()
{
	return this->magicWandSprite;
}

EquipmentType MagicWand::getID()
{
	return this->ID;
}

float MagicWand::getDamage()
{
	return this->damage;
}

sf::Texture MagicWand::magicWandTexture;