#pragma once

#include "Bullet.hpp"
#include "PoolManager.hpp"


class BulletManager : public PoolManager<Bullet>
{
public:
	void Spawn(Vector2 pos, float angleDeg);
};