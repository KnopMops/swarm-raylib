#include "EnemyManager.hpp"
#include "GameConfig.hpp"
#include "Player.hpp"

#include "raylib.h"
#include "raymath.h"


void EnemyManager::Init(Player *player)
{
	_player = player;
}

Vector2 EnemyManager::pickSpawnPoint() const
{
	return RandomSpawnPoint(_player->GetPosition(), 200.0f);
}

void EnemyManager::SpawnBatch(int count)
{
	_batchRemaining = count;
	_staggerTimer = 0.0f;
}

void EnemyManager::Spawn(Vector2 pos)
{
	auto* enemy = spawnInPool();
	enemy->Activate(pos);
	enemy->SetPlayer(_player);
}

void EnemyManager::Update(float dt)
{
	if (_batchRemaining > 0)
	{
		_staggerTimer -= dt;

		if (_staggerTimer < 0.0f)
		{
			_staggerTimer = _staggerInterval;
			_batchRemaining--;

			Spawn(pickSpawnPoint());
		}
	}

	PoolManager<Enemy>::Update(dt);
}