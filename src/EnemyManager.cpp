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
	constexpr float MIN_DIST = 300.0f;
	constexpr float MIN_DIST_SQ = MIN_DIST * MIN_DIST;

	Vector2 playerPos = _player->GetPosition();
	Vector2 candidate;

	do {
		candidate.x = RandomFloat(0.0f, GameConfig::MAP_W);
		candidate.y = RandomFloat(0.0f, GameConfig::MAP_H);
	} while (Vector2DistanceSqr(candidate, playerPos) < MIN_DIST_SQ);

	return candidate;
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