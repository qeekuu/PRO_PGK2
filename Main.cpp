#include <SFML/Graphics.hpp>
#include <cmath>
#include "Player.h"
#include "Enemy.h"
#include "Fly.h"
#include <vector>
#include <iostream>

int main()
{
    enum class GameState 
    { 
        Playing, 
        GameOver 
    };
    sf::Vector2u wSize{ 800, 600 };
    sf::RenderWindow window(sf::VideoMode(wSize),"GRA");
    window.setVerticalSyncEnabled(true);

    GameState gameState = GameState::Playing;
    std::unique_ptr<Player> player = std::make_unique<Player>(window.getSize());
    DIR direction = player->getPlayerDirection();
    std::vector<std::unique_ptr<Enemy>> Flys;
    sf::Clock waveTime;

    sf::Clock damageTextClock;
    const float damageDisplayDuration = 0.1f; 
    int lastDamage = 0;
    bool showDamageText = false;

    sf::Clock damageTextClockMonster;
    int lastDamageMonster = 0;
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

                    Flys.erase(Flys.begin() + i);
                }
            }
            //end game section
            if (player->getHp() <= 0)
            {
                gameState = GameState::GameOver;
            }
        }
        //Drawing section
        window.clear(sf::Color::Black);
        window.draw(playerHpText);
        if (gameState == GameState::Playing)
        {
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
        }
        else
        {
            window.draw(gameOverText);
        }

        window.display();
    }

    return 0;
}