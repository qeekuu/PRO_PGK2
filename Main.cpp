#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Network.hpp>
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
#include "UpgradePlayer.h"
#include "Bone.h"
#include "Item_SI.h"
#include "NetworkManager.h"
#include <unordered_set>


sf::Vector2f getRandomOffscreenPosition(const sf::View& view, float minOffset = 30.f, float maxOffset = 150.f);
void spawnEnemy(float x, float y);
NetworkManager net;
std::vector<std::shared_ptr<Enemy>> enemies;

static uint32_t nextEnemyID = 0;
// Terry Davis
sf::Vector2f gameOverSpriteVelocity{ 2.f, 2.f };
static std::unordered_map<int, std::shared_ptr<Enemy>> enemyMap;
std::unordered_set<int> receivedIds;
int main(int argc, char** argv)
{
    // NET Section
    bool netOk = true;
    if (argc >= 2 && std::string(argv[1]) == "--server") 
    {
        netOk = net.startServer(55001);
    }
    else if (argc >= 3 && std::string(argv[1]) == "--client") 
    {
    
        auto serverIpOpt = sf::IpAddress::resolve(argv[2]);
        if (!serverIpOpt) {
            std::cerr << "Niepoprawny adres IP: " << argv[2] << "\n";
            return EXIT_FAILURE;
        }
        
        netOk = net.startClient(*serverIpOpt, 55001);
    }
    if (!netOk) {
        std::cerr << "B³¹d po³¹czenia sieciowego\n";
        return EXIT_FAILURE;
    }

    // game map section

    sf::Texture tileset;
    if (!tileset.loadFromFile("Tile_0v1.png"))
    {
        throw std::runtime_error("error during loading a tileset texture");
    }

    std::vector<std::vector<int>> mapData = 
    {
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    };

    const unsigned tileSize = 128;
    const unsigned width = mapData[0].size();
    const unsigned height = mapData.size();

    std::vector<sf::Sprite> tiles;
    tiles.reserve(width * height);

    for (unsigned y = 0; y < height; ++y) 
    {
        for (unsigned x = 0; x < width; ++x) 
        {
            int id = mapData[y][x];

            int tu = id % (tileset.getSize().x / tileSize);
            int tv = id / (tileset.getSize().x / tileSize);

            sf::Sprite spr{ tileset };
            spr.setTexture(tileset);
            spr.setTextureRect(sf::IntRect({ static_cast<int>(tu * tileSize), static_cast<int>(tv * tileSize) },{ static_cast<int>(tileSize),static_cast<int>(tileSize) }));
            spr.setPosition({ static_cast<float>(x * tileSize),static_cast<float>(y * tileSize) });
            tiles.push_back(spr);
        }
    }

    float mapWidth = width * static_cast<float>(tileSize);
    float mapHeight = height * static_cast<float>(tileSize);

    enum class GameState 
    { 
        Playing, 
        GameOver,
		Inventory,
		WaveUpgrade
    };
    sf::Vector2u wSize{ 800, 600 };
    sf::RenderWindow window(sf::VideoMode(wSize),"GRA");
    // Views for controlling camera
    sf::View mainView(sf::FloatRect( { 0.f, 0.f } , {static_cast<float>(wSize.x) ,  static_cast<float>(wSize.y) } ) );

    window.setVerticalSyncEnabled(true);
	unsigned int waveCounter = 0;
    unsigned int mikrofala = 0;
    std::vector<std::unique_ptr<Items>> globalMapItems;
    std::unique_ptr<UpgradePlayer> upgradeManager= std::make_unique<UpgradePlayer>();
    // rectangle for upgradeManager_1
    sf::RectangleShape box1;
    // rectangle for upgradeManager_2
    sf::RectangleShape box2;
    // rectangle for upgradeManager_3
    sf::RectangleShape box3;


    GameState gameState = GameState::Playing;
    std::unique_ptr<Player> player = std::make_unique<Player>(window.getSize());
    Inventory& inv = player->getInventory();
    DIR direction = player->getPlayerDirection();
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

    sf::Text upgradeManager_1(font);
    upgradeManager_1.setCharacterSize(16);
    upgradeManager_1.setFillColor(sf::Color::Red);


    sf::Text upgradeManager_2(font);
    upgradeManager_2.setCharacterSize(16);
    upgradeManager_2.setFillColor(sf::Color::Red);


    sf::Text upgradeManager_3(font);
    upgradeManager_3.setCharacterSize(16);
    upgradeManager_3.setFillColor(sf::Color::Red);


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
                    player->reset();
                    expBarCounter.setSize({ 0,16 });
                    player = std::make_unique<Player>(window.getSize());
                    enemies.clear();
                    globalMapItems.clear();
                    player->getEquipment().clear();
                    player->getInventory().getEQ().clear();
                    waveTime.restart();
                    gameState = GameState::Playing;
                    mikrofala = 0;
                    
                }

				if(net.mode() == NetworkManager::Mode::Server && gameState == GameState::Playing && keyPressed->scancode == sf::Keyboard::Scancode::I)
				{
					gameState = GameState::Inventory;
				}else if(net.mode() == NetworkManager::Mode::Server && gameState == GameState::Inventory && keyPressed->scancode == sf::Keyboard::Scancode::I)
				{
					gameState = GameState::Playing;
				}

                if (net.mode() == NetworkManager::Mode::Server && gameState == GameState::Inventory)
                {
                    
                    if (keyPressed->scancode == sf::Keyboard::Scancode::W)
                    {
                        inv.moveSelection(-1, 0);
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::S)
                    {
                        inv.moveSelection(1, 0);
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::A)
                    {
                        inv.moveSelection(0, -1);
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::D)
                    {
                        inv.moveSelection(0, 1);
                    }
                    else if (keyPressed->scancode == sf::Keyboard::Scancode::Enter)
                    {
                        inv.toggleItem(*player);
                    }
                }
				
            }
            else if (auto* mb = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (net.mode() == NetworkManager::Mode::Server && gameState == GameState::Playing)
                {
                    if (mb->button == sf::Mouse::Button::Left)
                    {
                        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
                        sf::Vector2f worldPos = window.mapPixelToCoords(pixelPos, mainView);
                        player->attackStart(worldPos, player->getPosition());
                    }
                }
                if (net.mode() == NetworkManager::Mode::Server && gameState == GameState::WaveUpgrade)
                {
                    if (upgradeManager_1.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
                    {
                        if (mb->button == sf::Mouse::Button::Left)
                        {
                            upgradeManager->applyUpgrade(0,*player);
                            waveCounter = 0;
                            gameState = GameState::Playing;
                        }
                    }
                    else if (upgradeManager_2.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
                    {
                        if (mb->button == sf::Mouse::Button::Left)
                        {
                            upgradeManager->applyUpgrade(1, *player);
                            waveCounter = 0;
                            gameState = GameState::Playing;
                        }
                    }
                    else if (upgradeManager_3.getGlobalBounds().contains(window.mapPixelToCoords(sf::Mouse::getPosition(window))))
                    {
                        if (mb->button == sf::Mouse::Button::Left)
                        {
                            upgradeManager->applyUpgrade(2, *player);
                            waveCounter = 0;
                            gameState = GameState::Playing;
                        }
                    }
                }
            }
        }


       
        net.tickReceive();
        if (net.mode() == NetworkManager::Mode::Server)
        {
            
            net.handleIncoming(spawnEnemy);
            
            net.broadcastState(enemies);
        }
        else if (net.mode() == NetworkManager::Mode::Client)
        {
           
            static std::unordered_map<int, std::shared_ptr<Enemy>> enemyMap;

            
            for (auto& pkt : net.getReceived())
            {
                PacketType type;
                pkt >> type;
                if (type != PacketType::StateUpdate)
                    continue;

                uint32_t count;
                pkt >> count;

                
                std::unordered_set<int> receivedIds;

            
                for (uint32_t i = 0; i < count; ++i)
                {
                    int id;
                    EnemyType etype;
                    float x, y;
                    
                    pkt >> id
                        >> etype
                        >> x >> y;

                    receivedIds.insert(id);

                    auto it = enemyMap.find(id);
                    if (it == enemyMap.end())
                    {
                        
                        std::shared_ptr<Enemy> e = nullptr;
                        if (etype == EnemyType::Fly)
                            e = std::make_shared<Fly>();
                        else if (etype == EnemyType::Bone)
                            e = std::make_shared<Bone>();

                        e->setID(id);
                        e->setPosition({ x, y });
                        enemyMap.emplace(id, e);
                    }
                    else
                    {
                        
                        it->second->setPosition({ x, y });
                    }
                }

               
                for (auto it = enemyMap.begin(); it != enemyMap.end(); )
                {
                    if (!receivedIds.count(it->first))
                        it = enemyMap.erase(it);
                    else
                        ++it;
                }

               
                enemies.clear();
                for (auto& [id, e] : enemyMap)
                    enemies.push_back(e);
            }

            net.clearReceived();
        }

        if (net.mode() == NetworkManager::Mode::Client) {
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                auto wp = window.mapPixelToCoords(sf::Mouse::getPosition(window), mainView);
                sf::Packet p;
                p << PacketType::SpawnEnemy
                    << wp.x << wp.y;
                net.sendPacket(p);
            }
        }



        if (gameState == GameState::Playing)
        {
            //enemies spawn section
            if (net.mode() == NetworkManager::Mode::Server && waveTime.getElapsedTime().asSeconds() >= 0.5f)
            {
                if (mikrofala <= 1)
                {
                    auto e = std::make_shared<Fly>();
                    e->setID(nextEnemyID++);
                    sf::Vector2f spawnPos = getRandomOffscreenPosition(mainView, static_cast<float>(player->getPosition().y));
                    e->setPosition(spawnPos);
                    enemies.push_back(e);
                    waveTime.restart();
                }
                else if (mikrofala > 1)
                {
                    auto e = std::make_shared<Bone>();
                    e->setID(nextEnemyID++);
                    sf::Vector2f spawnPos = getRandomOffscreenPosition(mainView, static_cast<float>(player->getPosition().y));
                    e->setPosition(spawnPos);
                    enemies.push_back(e);
                    waveTime.restart();
                }
                else
                {
                    // TUTAJ NIC SIE NIE DZIEJE BEJT HAHAHAHAHAH WITAMY W RADMOMIU NIC TU NIE MA
                }
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
            for (int i = 0;i < enemies.size(); i++)
            {
                enemies[i]->move(player->getPosition());
            }
            //Animations section
            player->update(direction, moving);
            for (int i = 0;i < enemies.size();i++)
            {
                enemies[i]->update(player->getPosition());
            }

            player->attackUpdate(sf::Mouse::getPosition(window), player->getPosition());
            //Collision section
            sf::FloatRect playerBoundingBox = player->getBoundingBox();
            for (int i = 0;i< globalMapItems.size();i++)
            {
                if (const std::optional intersection = playerBoundingBox.findIntersection(globalMapItems[i]->getBoundingBox()))
                {
                    
                    player->getInventory().addItem(std::move(globalMapItems[i]));
                    globalMapItems.erase(globalMapItems.begin() + i);
                    i--;
                }
            }
            
            
            for (int i = 0;i < enemies.size(); i++)
            {
                sf::FloatRect flyBoundingBox = enemies[i]->getBoundingBox();
                if (const std::optional intersection = playerBoundingBox.findIntersection(flyBoundingBox))
                {
                    if (enemies[i]->canDealDamage())
                    {
                        player->setHp(enemies[i]->getDamage());
                        enemies[i]->notifyDamageDealt();

                        lastDamage = enemies[i]->getDamage();
                        damageDealth.setString(std::to_string(lastDamage));
                        damageTextClock.restart();
                        showDamageText = true;
                    }
                }
            }
            for (int i = 0;i < enemies.size(); i++)
            {
                for (int j = 0;j < player->magicAttackVector().size();j++)
                {
                    if (const std::optional intersection = player->magicAttackVector()[j]->getBoundingBox().findIntersection(enemies[i]->getBoundingBox()))
                    {
                        damageDealthMonster.setPosition({ enemies[i]->getPosition().x,enemies[i]->getPosition().y - 32 });
                        lastDamageMonster = player->magicAttackVector()[j]->getDamage();
                        damageDealthMonster.setString(std::to_string(lastDamageMonster));
                        damageTextClockMonster.restart();
                        showDamageTextMonster = true;

                        enemies[i]->setHp(player->magicAttackVector()[j]->getDamage());
                        player->magicAttackVector().erase(player->magicAttackVector().begin() + j);
                        break;
                    }
                }
            }
            for (int i = 0; i < enemies.size(); i++)
            {
                if (enemies[i]->getHp() <= 0)
                {
                    std::srand(time(0));
                    int temp = rand() % 3;
                    if (temp == 1)
                    {
                        auto item = std::make_unique<MagicWand>();
                        item->getSprite().setPosition(enemies[i]->getPosition());
                        globalMapItems.push_back(std::move(item));
                    }
                    else if (temp == 2)
                    {
                        auto item = std::make_unique<Item_SI>();
                        item->getSprite().setPosition(enemies[i]->getPosition());
                        globalMapItems.push_back(std::move(item));
                    }

                    expBarCounterFloat += 20;
                    player->setXp(5);
                    if (player->getXp() >= player->getTreshold())
                    {
                        player->setLevel(1);
                        player->setDamage(0.25f);
                        player->setArmor(0.25f);
                        expBarCounterFloat = 0;
                        player->setTreshold(100);
                    }
                    expBarCounter.setSize(sf::Vector2f(expBarCounterFloat,16));
                    enemies.erase(enemies.begin() + i);
                    waveCounter++;
                    mikrofala++; // W Pythonie nie ma inkrementacji na szczescie uzywamy C++ :) (WAVE COUNTER 2)

                }
            }
            //end game section
            if (player->getHp() <= 0)
            {
                gameState = GameState::GameOver;
            }
			

			if(waveCounter > 2)
			{
				gameState = GameState::WaveUpgrade;
			}
        }

        //Setting the view
        float halfWView = mainView.getSize().x / 2.f;
        float halfHView = mainView.getSize().y / 2.f;
        sf::Vector2f center = player->getPosition();
        float minX = halfWView;
        float minY = halfHView;
        float maxX = std::max(mapWidth - halfWView, halfWView);
        float maxY = std::max(mapHeight - halfHView, halfHView);
        center.x = std::clamp(center.x, minX, maxX);
        center.y = std::clamp(center.y, minY, maxY);
        mainView.setCenter(center);

        //Drawing section
        window.clear(sf::Color::Black);
        window.setView(mainView);
        for (int i = 0;i < tiles.size(); i++)
        {
            window.draw(tiles[i]);
        }

        if (gameState == GameState::Playing)
        {
            window.setView(mainView);
            player->draw(window);
            for (int i = 0;i < enemies.size(); i++)
            {
                enemies[i]->draw(window);
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
            window.setView(window.getDefaultView());
            window.draw(playerHpText);
            window.draw(expBar);
            window.draw(expBarCounter);
        }
        else if(gameState == GameState::GameOver)
        {
            window.setView(window.getDefaultView());
            window.draw(gameOverText);
            
            
        }
		else if(gameState == GameState::Inventory)
		{
            window.setView(mainView);
			player->draw(window);
            for (int i = 0;i < enemies.size(); i++)
            {
                enemies[i]->draw(window);
            }
			player->attackDraw(window);	


            window.setView(window.getDefaultView());
            player->drawEQ(window);

		}else if(gameState == GameState::WaveUpgrade)
		{
			//window.draw(WaveUpgradeTestScreen);
            upgradeManager->randUpgrades();

            upgradeManager_1.setString(upgradeManager->getWaveUpgrades()[0]->getName());

            upgradeManager_2.setString(upgradeManager->getWaveUpgrades()[1]->getName());

            upgradeManager_3.setString(upgradeManager->getWaveUpgrades()[2]->getName());

            sf::FloatRect bounds1 = upgradeManager_1.getLocalBounds();
            sf::FloatRect bounds2 = upgradeManager_2.getLocalBounds();
            sf::FloatRect bounds3 = upgradeManager_3.getLocalBounds();

            const float gap = 40.f;
            float totalWidth = bounds1.size.x + bounds2.size.x + bounds3.size.x + 2 * gap;

            float startX = (float(wSize.x) - totalWidth) / 2.f;
            float centerY = float(wSize.y) / 2.f;

            // set text position
            upgradeManager_1.setPosition({ startX, centerY - bounds1.size.y / 2.f });
            upgradeManager_2.setPosition({ startX + bounds1.size.x + gap, centerY - bounds2.size.y / 2.f });
            upgradeManager_3.setPosition({ startX + bounds1.size.x + gap + bounds2.size.x + gap, centerY - bounds3.size.y / 2.f });

            float padding = 10.f;
            float maxHeight = std::max({ bounds1.size.x, bounds2.size.y, bounds3.size.y }) + 2 * padding;

            float ambientMargin = 40.f;
            float ambientWidth = totalWidth + ambientMargin;
            float ambientHeight = maxHeight + ambientMargin;

            float ambientX = (float(wSize.x) - ambientWidth) / 2.f;
            float ambientY = (float(wSize.y) - ambientHeight) / 2.f;

            sf::RectangleShape ambient;
            ambient.setSize({ ambientWidth, ambientHeight });
            ambient.setFillColor(sf::Color(20, 20, 20));
            ambient.setOutlineThickness(1);
            ambient.setOutlineColor(sf::Color(150, 150, 150));
            ambient.setPosition({ ambientX, ambientY } );

            
            box1.setSize({ bounds1.size.x + 2 * padding, bounds1.size.y + 2 * padding });
            box1.setPosition({ upgradeManager_1.getPosition().x + bounds1.position.x - padding,
                              upgradeManager_1.getPosition().y + bounds1.position.y - padding });
            box1.setFillColor(sf::Color(50, 50, 50));
            box1.setOutlineThickness(1);
            box1.setOutlineColor(sf::Color(150, 150, 150));

            
            box2.setSize({ bounds2.size.x + 2 * padding, bounds2.size.y + 2 * padding });
            box2.setPosition({ upgradeManager_2.getPosition().x + bounds2.position.x - padding,
                              upgradeManager_2.getPosition().y + bounds2.position.y - padding });
            box2.setFillColor(sf::Color(50, 50, 50));
            box2.setOutlineThickness(1);
            box2.setOutlineColor(sf::Color(150, 150, 150));

            
            box3.setSize({ bounds3.size.x + 2 * padding, bounds3.size.y + 2 * padding });
            box3.setPosition({ upgradeManager_3.getPosition().x + bounds3.position.x - padding,
                              upgradeManager_3.getPosition().y + bounds3.position.y - padding });
            box3.setFillColor(sf::Color(50, 50, 50));
            box3.setOutlineThickness(1);
            box3.setOutlineColor(sf::Color(150, 150, 150));

            window.setView(window.getDefaultView());
            window.draw(ambient);

            window.draw(box1);
            window.draw(upgradeManager_1);

            window.draw(box2);
            window.draw(upgradeManager_2);

            window.draw(box3);
            window.draw(upgradeManager_3);
		}

        window.display();
    }

    return 0;
}

sf::Vector2f getRandomOffscreenPosition(const sf::View& view, float minOffset, float maxOffset)
{
    sf::Vector2f center = view.getCenter();
    sf::Vector2f size = view.getSize();
    float left = center.x - size.x / 2.f;
    float top = center.y - size.y / 2.f;
    float right = center.x + size.x / 2.f;
    float bottom = center.y + size.y / 2.f;

    int side = rand() % 4;
    float x, y;

    switch (side)
    {
    case 0: 
        x = left - (minOffset + rand() % int(maxOffset - minOffset));
        y = top + rand() % int(size.y);
        break;
    case 1: 
        x = right + (minOffset + rand() % int(maxOffset - minOffset));
        y = top + rand() % int(size.y);
        break;
    case 2: 
        x = left + rand() % int(size.x);
        y = top - (minOffset + rand() % int(maxOffset - minOffset));
        break;
    case 3: 
        x = left + rand() % int(size.x);
        y = bottom + (minOffset + rand() % int(maxOffset - minOffset));
        break;
    }
    return sf::Vector2f(x, y);
}

void spawnEnemy(float x, float y) {
    static bool toggle = false;
    if (!toggle) {
        auto e = std::make_shared<Fly>();
        e->setID(nextEnemyID++);
        e->setPosition({ x, y });
        enemies.push_back(e);
    }
    else {
        auto e = std::make_shared<Bone>();
        e->setID(nextEnemyID++);
        e->setPosition({ x, y });
        enemies.push_back(e);
    }
    toggle = !toggle;
}
