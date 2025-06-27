#include "UpgradePlayer.h"

UpgradePlayer::UpgradePlayer()
{
	upgrades.push_back(std::make_unique<Upgrade>(UpgradeType::DamageBonus, "Damage Bonus +0.5", 0.5));
	upgrades.push_back(std::make_unique<Upgrade>(UpgradeType::HealthBonus, "Health Bonus +10", 10));
	upgrades.push_back(std::make_unique<Upgrade>(UpgradeType::ArmorBonus, "Armor Bonus +0.5", 0.5));
	upgrades.push_back(std::make_unique<Upgrade>(UpgradeType::SpeedBonus, "Speed Bonus +0.10", 0.5));
	std::srand(std::time(0));
}


void UpgradePlayer::randUpgrades()
{
	for (int i = 0; i < 3; i++)
	{
		unsigned int randomValue = rand() % 3;
		waveUpgrades.push_back(std::make_unique<Upgrade>(*upgrades[randomValue]));
	}
}

void UpgradePlayer::applyUpgrade(int choice, Player& player)
{
	switch (waveUpgrades[choice]->getType())
	{
	case UpgradeType::DamageBonus:
		player.setDamage(0.5f);
		break;
	case UpgradeType::ArmorBonus:
		player.setArmor(0.5f);
		break;
	case UpgradeType::HealthBonus:
		player.setHPUpgrade(10.0f);
		break;
	case UpgradeType::SpeedBonus:
		player.setSpeed(0.1f);
		break;
	default:
		break;
	}
}



