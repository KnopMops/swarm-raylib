#pragma once

#include "Bullet.hpp"
#include "PoolManager.hpp"

#include "vector"
#include "memory"


class BulletManager : public PoolManager<Bullet>
{
public:
	void Spawn(Vector2 pos, float angleDeg);
};