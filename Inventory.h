#pragma once
#include <vector>
#include <iostream>
#include "Items.h"
#include "EmptyItemSlot.h"
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <memory>
class Player;

class Inventory
{
public:
	Inventory();
	void draw(sf::RenderWindow& window);
	void addItem(std::unique_ptr<Items> item);
	void moveSelection(int dRow, int dCol);
	void toggleItem(Player& player);
private:
	std::vector<std::unique_ptr<Items>> EQ;
	int selectedSlot = 0;
	static constexpr int columns = 5;
	static constexpr int rows = 4;
};
