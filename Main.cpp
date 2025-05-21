#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <cmath>
#include "Player.h"
#include "Enemy.h"
#include "Fly.h"
#include <vector>
#include <iostream>
#include "Items.h"
#include "MagicWand.h"
#include "EmptyItemSlot.h"
#include "Inventory.h"
int main()
{
    enum class GameState 
    { 
        Playing, 
        GameOver,
		Inventory,
		WaveUpgrade
    };
    sf::Vector2u wSize{ 800, 600 };
    sf::RenderWindow window(sf::VideoMode(wSize),"GRA");
    window.setVerticalSyncEnabled(true);
	unsigned int waveCounter = 0;

    std::vector<std::unique_ptr<Items>> globalMapItems;

    GameState gameState = GameState::Playing;
    std::unique_ptr<Player> player = std::make_unique<Player>(window.getSize());
    Inventory& inv = player->getInventory();
    DIR direction = player->getPlayerDirection();
    std::vector<std::unique_ptr<Enemy>> Flys;
    sf::Clock waveTime;
  
    sf::Clock damageTextClock;
    const float damageDisplayDuration = 0.1f; 
    int lastDamage = 0;
    bool showDamageText = false;

    sf::Clock damageTextClockMonster;
    float lastDamageMonster = 0;
    bool showDamageTextMonster = false;


    sf::Font font;
    if (!font.openFromFile("arial.ttf"))
    {
        throw std::runtime_error("Nie udalo sie wczytac czcionki");
    }
    sf::Text gameOverText(font);
    gameOverText.setString("GAME OVER\nPress R to restart\nEsc to quit");
    gameOverText.setCharacterSize(24);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition({300,200});


    sf::Text playerHpText(font);
    playerHpText.setCharacterSize(24);
    playerHpText.setFillColor(sf::Color::Red);
    playerHpText.setPosition({ 0,0 });

    sf::Text damageDealth(font);
    damageDealth.setCharacterSize(16);
    damageDealth.setFillColor(sf::Color::Red);

    sf::Text damageDealthMonster(font);
    damageDealthMonster.setCharacterSize(16);
    damageDealthMonster.setFillColor(sf::Color::White);


	sf::Text inventoryTestScreen(font);
	inventoryTestScreen.setCharacterSize(16);
	inventoryTestScreen.setFillColor(sf::Color::Red);
	inventoryTestScreen.setString("inventoryHUDTest");
	inventoryTestScreen.setPosition({300,200});


	sf::Text WaveUpgradeTestScreen(font);
	WaveUpgradeTestScreen.setCharacterSize(16);
	WaveUpgradeTestScreen.setFillColor(sf::Color::Red);
	WaveUpgradeTestScreen.setString("WaveUpgrade_TEST");
	WaveUpgradeTestScreen.setPosition({300,200});

    sf::RectangleShape expBar(sf::Vector2f(400, 16));
    expBar.setPosition({300,5});
    expBar.setFillColor(sf::Color(80, 80, 80));
    expBar.setOutlineThickness(1);
    expBar.setOutlineColor(sf::Color(150, 150, 150));

    float expBarCounterFloat = 0;
    sf::RectangleShape expBarCounter(sf::Vector2f(expBarCounterFloat, 16));
    expBarCounter.setPosition({ 300,5 });
    expBarCounter.setFillColor(sf::Color(0, 0, 255));
    expBarCounter.setOutlineThickness(1);
    expBarCounter.setOutlineColor(sf::Color(150, 150, 150));
    

    waveTime.start();
    while (window.isOpen())
    {
        playerHpText.setString("PLAYER HP: " + std::to_string(player->getHp()));
        damageDealth.setPosition({player->getPosition().x-5,player->getPosition().y - 32});
        //Event section
        while (auto event = window.pollEvent())  
        {

            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                {
                    window.close();
                }

                if (gameState == GameState::GameOver && keyPressed->scancode == sf::Keyboard::Scancode::R)
                {
                    player = std::make_unique<Player>(window.getSize());
                    Flys.clear();
                    waveTime.restart();
                    gameState = GameState::Playing;
                }

				if(gameState == GameState::Playing && keyPressed->scancode == sf::Keyboard::Scancode::I)
				{
					gameState = GameState::Inventory;
				}else if(gameState == GameState::Inventory && keyPressed->scancode == sf::Keyboard::Scancode::I)
				{
					gameState = GameState::Playing;
				}

                if (gameState == GameState::Inventory) {
                    // W/S/A/D
                    if (keyPressed->scancode == sf::Keyboard::Scancode::W) inv.moveSelection(-1, 0);
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::S) inv.moveSelection(1, 0);
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::A) inv.moveSelection(0, -1);
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::D) inv.moveSelection(0, 1);
                    // ENTER
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
                        inv.toggleItem(*player);
                }
				
            }
            else if (auto* mb = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (gameState == GameState::Playing)
                {
                    if (mb->button == sf::Mouse::Button::Left)
                    {
                        player->attackStart(sf::Mouse::getPosition(window), player->getPosition());
                    }
                }

            }
        }

        if (gameState == GameState::Playing)
        {
            //Flys spawn section
            if (waveTime.getElapsedTime().asSeconds() >= 1.5f)
            {
                Flys.push_back(std::make_unique<Fly>());
                waveTime.restart();
            }


            //Entitys moving section
            sf::Vector2f movement{ 0.f, 0.f };
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            {
                movement.x -= player->getSpeed();
                direction = DIR::LEFT;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            {
                movement.x += player->getSpeed();
                direction = DIR::RIGHT;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            {
                movement.y -= player->getSpeed();
                direction = DIR::UP;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            {
                movement.y += player->getSpeed();
                direction = DIR::DOWN;
            }

            const bool moving = (movement.x != 0.f || movement.y != 0.f);
            if (moving)
            {
                float length = std::sqrt(movement.x * movement.x + movement.y * movement.y);
                movement /= length;
                movement *= player->getSpeed();
                player->move(movement);
            }
            for (int i = 0;i < Flys.size(); i++)
            {
                Flys[i]->move(player->getPosition());
            }
            //Animations section
            player->update(direction, moving);
            for (int i = 0;i < Flys.size();i++)
            {
                Flys[i]->update(player->getPosition());
            }

            player->attackUpdate(sf::Mouse::getPosition(window), player->getPosition());
            //Collision section
            sf::FloatRect playerBoundingBox = player->getBoundingBox();
            //***********************************TO_DO*************************************************************
            for (int i = 0;i< globalMapItems.size();i++)
            {
                if (const std::optional intersection = playerBoundingBox.findIntersection(globalMapItems[i]->getBoundingBox()))
                {
                    
                    player->getInventory().addItem(std::move(globalMapItems[i]));
                    globalMapItems.erase(globalMapItems.begin() + i);
                    i--;
                }
            }
            
            
            for (int i = 0;i < Flys.size(); i++)
            {
                sf::FloatRect flyBoundingBox = Flys[i]->getBoundingBox();
                if (const std::optional intersection = playerBoundingBox.findIntersection(flyBoundingBox))
                {
                    if (Flys[i]->canDealDamage())
                    {
                        player->setHp(Flys[i]->getDamage());
                        Flys[i]->notifyDamageDealt();

                        lastDamage = Flys[i]->getDamage();
                        damageDealth.setString(std::to_string(lastDamage));
                        damageTextClock.restart();
                        showDamageText = true;
                    }
                }
            }
            for (int i = 0;i < Flys.size(); i++)
            {
                for (int j = 0;j < player->magicAttackVector().size();j++)
                {
                    if (const std::optional intersection = player->magicAttackVector()[j]->getBoundingBox().findIntersection(Flys[i]->getBoundingBox()))
                    {
                        damageDealthMonster.setPosition({ Flys[i]->getPosition().x,Flys[i]->getPosition().y - 32 });
                        lastDamageMonster = player->magicAttackVector()[j]->getDamage();
                        damageDealthMonster.setString(std::to_string(lastDamageMonster));
                        damageTextClockMonster.restart();
                        showDamageTextMonster = true;

                        Flys[i]->setHp(player->magicAttackVector()[j]->getDamage());
                        player->magicAttackVector().erase(player->magicAttackVector().begin() + j);
                        break;
                    }
                }
            }
            for (int i = 0; i < Flys.size(); i++)
            {
                if (Flys[i]->getHp() <= 0)
                {
                    auto item = std::make_unique<MagicWand>();
                    item->getSprite().setPosition(Flys[i]->getPosition());
                    globalMapItems.push_back(std::move(item));
                    expBarCounterFloat += 20;
                    player->setXp(5);
                    if (player->getXp() >= player->getTreshold())
                    {
                        //**************************************************TO_DO***********************************************************
                        player->setLevel(1);
                        player->setDamage(0.25f);
                        player->setArmor(0.25f);
                        expBarCounterFloat = 0;
                        player->setTreshold(100);
                    }
                    expBarCounter.setSize(sf::Vector2f(expBarCounterFloat,16));
                    Flys.erase(Flys.begin() + i);
                    //waveCounter++;

                }
            }
            //end game section
            if (player->getHp() <= 0)
            {
                gameState = GameState::GameOver;
            }
			

			if(waveCounter > 10)
			{
				gameState = GameState::WaveUpgrade;
			}
        }
        //Drawing section
        window.clear(sf::Color::Black);
        window.draw(playerHpText);
        if (gameState == GameState::Playing)
        {
            window.draw(expBar);
            window.draw(expBarCounter);
            player->draw(window);
            for (int i = 0;i < Flys.size(); i++)
            {
                Flys[i]->draw(window);
            }
            player->attackDraw(window);
            if (showDamageText && damageTextClock.getElapsedTime().asSeconds() <= damageDisplayDuration)
            {
                damageDealth.setPosition({ player->getPosition().x - 5, player->getPosition().y - 32 });
                window.draw(damageDealth);
            }
            else
            {
                showDamageText = false;
            }

            if (showDamageTextMonster && damageTextClockMonster.getElapsedTime().asSeconds() <= damageDisplayDuration)
            {
                window.draw(damageDealthMonster);
            }
            else
            {
                showDamageTextMonster = false;
            }

            for (auto& item : globalMapItems)
            {
                item->draw(window);
            }
        }
        else if(gameState == GameState::GameOver)
        {
            window.draw(gameOverText);
        }
		else if(gameState == GameState::Inventory)
		{
			window.draw(inventoryTestScreen);
			player->draw(window);
            for (int i = 0;i < Flys.size(); i++)
            {
                Flys[i]->draw(window);
            }
			player->attackDraw(window);	
            
            player->drawEQ(window);
            

		}else if(gameState == GameState::WaveUpgrade)
		{
			window.draw(WaveUpgradeTestScreen);
		}


        window.display();
    }

    return 0;
}
