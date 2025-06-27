#include "Inventory.h"
#include "Player.h"

Inventory::Inventory()
{
    
}

void Inventory::draw(sf::RenderWindow& window)
{
    // drawing ambient
    sf::RectangleShape overlay(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    // drawing main panel
    sf::RectangleShape panel(sf::Vector2f(600, 400));
    panel.setPosition({ 100, 100 });
    panel.setFillColor(sf::Color(50, 50, 50, 220));
    panel.setOutlineThickness(2);
    panel.setOutlineColor(sf::Color::White);
    window.draw(panel);

    // drawing slots
    int slotIndex = 0;
    for (int row = 0; row < rows; ++row) 
    {
        for (int col = 0; col < columns; ++col) 
        {
            sf::Vector2f pos = { 120 + col * 70.f, 120 + row * 70.f };
            sf::RectangleShape slot(sf::Vector2f(64, 64));
            slot.setPosition(pos);
            slot.setFillColor(sf::Color(80, 80, 80));
            slot.setOutlineThickness(1);
            slot.setOutlineColor(sf::Color(150, 150, 150));
            window.draw(slot);

            if (slotIndex < EQ.size() && EQ[slotIndex])
            {
                EQ[slotIndex]->getSprite().setPosition({ pos.x + 32, pos.y + 32 });
                EQ[slotIndex]->draw(window);
            }
            slotIndex++;
        }
    }
    // slot highlights
    int row = selectedSlot / columns;
    int col = selectedSlot % columns;
    sf::RectangleShape highlight({ 64, 64 });
    highlight.setPosition({ 120 + col * 70.f, 120 + row * 70.f });
    highlight.setFillColor(sf::Color(255, 255, 255, 100));
    window.draw(highlight);
}


void Inventory::addItem(std::unique_ptr<Items> item)
{
    if (EQ.size() >= 20)
    {
        return;
    }
    EQ.push_back(std::move(item));
}


void Inventory::moveSelection(int dRow, int dCol) 
{
    int row = selectedSlot / columns;
    int col = selectedSlot % columns;
    row = std::clamp(row + dRow, 0, rows - 1);
    col = std::clamp(col + dCol, 0, columns - 1);
    int idx = row * columns + col;
    if (idx < static_cast<int>(EQ.size()))
    {
        selectedSlot = idx;
    }
}

void Inventory::toggleItem(Player& player) 
{
    if (selectedSlot >= static_cast<int>(EQ.size()) || !EQ[selectedSlot])
    {
        return;
    }
    EquipmentType id = EQ[selectedSlot]->getID();
    auto it = player.getEquipment().find(id);

    if (it == player.getEquipment().end())
    {
        player.getEquipment()[id] = std::move(EQ[selectedSlot]);
        player.getInventory().EQ.erase(EQ.begin() + selectedSlot);
        player.setDamage(player.getEquipment().find(EquipmentType::WEAPON)->second->getDamage());
    }
    else
    {
        std::swap(EQ[selectedSlot], it->second);
    }
    

}

std::vector<std::unique_ptr<Items>> &Inventory::getEQ()
{
    return this->EQ;
}