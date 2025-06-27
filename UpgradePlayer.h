#pragma once
#include <vector>
#include <memory>
#include "Player.h"
#include "Upgrade.h"
#include <random>
class UpgradePlayer
{
public:
	UpgradePlayer();

	void applyUpgrade(int choice, Player& player);

	void randUpgrades();

	std::vector<std::unique_ptr<Upgrade>> &getWaveUpgrades()
	{
		return this->waveUpgrades;
	}
private:
	std::vector<std::unique_ptr<Upgrade>> upgrades;
	std::vector<std::unique_ptr<Upgrade>> waveUpgrades;
};

