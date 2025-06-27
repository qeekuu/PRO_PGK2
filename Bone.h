#pragma once
#include "Enemy.h"
#include "NetworkCommon.h"
class Bone : public Enemy
{
public:
	Bone();
	void draw(sf::RenderWindow& window) override;
	void update(sf::Vector2f playerPosition) override;
	void move(sf::Vector2f playerPosition) override;
	unsigned int getId() override;
	void setID(unsigned int id) override;
	sf::FloatRect getBoundingBox() override;
	void setHp(float damage) override;
	float getHp() override;
	unsigned int getDamage() override;
	bool canDealDamage() override;
	void notifyDamageDealt() override;
	sf::Vector2f getPosition() override;
	void setPosition(sf::Vector2f pos) override;
	EnemyType getType() override { return EnemyType::Bone; }
private:
	static unsigned int		nextID;
	static sf::Texture		sharedTexture;
	unsigned int			ID;
	float					hp{ 15.0f };
	sf::Sprite				sprite{ sharedTexture };
	unsigned int			currentFrame{ 0 };
	sf::Clock				animationClock;
	float					switchTime{ 0.07f };
	unsigned int			maxFrames{ 0 };
	float					speed{ 1.27485f };
	unsigned int			damage{ 5 };
	sf::Clock damageClock;
	static const float DAMAGE_COOLDOWN;
};

