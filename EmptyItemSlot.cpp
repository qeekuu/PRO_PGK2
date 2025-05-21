#include "EmptyItemSlot.h"


EmptyItemSlot::EmptyItemSlot()
{
	if (EmptyItemSlotTexture.getSize().x == 0)
	{
		if (!EmptyItemSlotTexture.loadFromFile("EmptyItemSlot.png"))
		{
			throw std::runtime_error("Error during loading a texture");
		}
		EmptyItemSlotTexture.setSmooth(true);
	}

	EmptyItemSlotSprite.setTexture(EmptyItemSlotTexture);
	EmptyItemSlotSprite.setTextureRect({{0,0}, {48,48}});
	EmptyItemSlotSprite.setOrigin({24,24});
	
}

void EmptyItemSlot::draw(sf::RenderWindow& window)
{
	window.draw(EmptyItemSlotSprite);
}

sf::Sprite &EmptyItemSlot::getSprite()
{
	return this->EmptyItemSlotSprite;
}

sf::FloatRect EmptyItemSlot::getBoundingBox()
{
	return this->EmptyItemSlotSprite.getGlobalBounds();
}

sf::Texture EmptyItemSlot::EmptyItemSlotTexture;