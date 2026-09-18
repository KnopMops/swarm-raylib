#pragma once

#include "HealthPotion.hpp"
#include "PoolManager.hpp"

#include "raylib.h"

#include "vector"
#include "memory"


class HealthPotionManager : public PoolManager<HealthPotion>
{
public:
	void Spawn(Vector2 pos);
};