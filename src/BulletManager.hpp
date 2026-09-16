#pragma once

#include "Bullet.hpp"

#include "vector"
#include "memory"


class BulletManager
{
public:
	void Spawn(Vector2 pos, float angleDeg);
	void Update(float dt);
	void Draw();

	void DeactivateAll();

	const std::vector<std::unique_ptr<Bullet>>& GetPool() const { return _pool; };

	int GetPoolTotal() const { return (int)_pool.size(); };
	int CountAlive() const {
		int n = 0;
		for (const auto& b : _pool) if (b->IsAlive()) n++;
		return n;
	}

private:
	std::vector<std::unique_ptr<Bullet>> _pool;
	float _bulletSpeed = 600.0f;
};