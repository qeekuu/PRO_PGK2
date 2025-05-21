#pragma once
#include "Enemy.h"
class Fly : public Enemy
{
public:
	Fly();
	void draw(sf::RenderWindow& window) override;
	void update(sf::Vector2f playerPosition) override;
	void move(sf::Vector2f playerPosition) override;
	unsigned int getId() override;
	sf::FloatRect getBoundingBox() override;
	void setHp(float damage) override;
	float getHp() override;
	unsigned int getDamage() override;
	bool canDealDamage() override;
	void notifyDamageDealt() override;
	sf::Vector2f getPosition() override;
private:
	static unsigned int		nextID;
	static sf::Texture		sharedTexture;
	unsigned int			ID;
	float					hp{ 10 };
	sf::Sprite				sprite{ sharedTexture };
	unsigned int			currentFrame{ 0 };
	sf::Clock				animationClock;
	float					switchTime{ 0.07f };
	unsigned int			maxFrames{ 0 };
	float					speed{ 1.9f };
	unsigned int			damage{ 2 };
	sf::Clock damageClock;
	static const float DAMAGE_COOLDOWN;
};

