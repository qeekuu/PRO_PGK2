#pragma once
#include <string>
#include "UpgradeType.h"
class Upgrade
{
public:
    Upgrade(UpgradeType u, std::string n, float v) : type{ u }, name{ n }, value{ v }
    {

    }

    Upgrade(const Upgrade& other) : type{ other.type }, name{ other.name }, value{ other.value } 
    {

    }

    UpgradeType getType()
    {
        return this->type;
    }

    std::string getName()
    {
        return this->name;
    }

    float getvalue()
    {
        return this->value;
    }
private:
    UpgradeType type;
    std::string name;
    float value;
};