#include "HealthPotionManager.hpp"

void HealthPotionManager::Spawn(Vector2 pos)
{
	auto* healthPotion = spawnInPool();
	healthPotion->Activate(pos);
}