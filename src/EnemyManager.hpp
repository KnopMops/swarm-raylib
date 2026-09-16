#pragma once

#include "Enemy.hpp"

#include "vector"
#include "memory"


class Player;

class EnemyManager
{
public:
	void Init(Player *player);
	void Spawn(Vector2 pos);
	void Update(float dt);
	void Draw();
	void DeactivateAll();

	const std::vector<std::unique_ptr<Enemy>>& GetPool() const { return _pool; };

	int GetPoolTotal() const { return (int)_pool.size(); };
	int CountAlive() const {
		int n = 0;
		for (const auto& b : _pool) if (b->IsAlive()) n++;
		return n;
	}

private:
	std::vector<std::unique_ptr<Enemy>> _pool;
	Player* _player = nullptr;
};