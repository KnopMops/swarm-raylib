#include "EnemyManager.hpp"
#include "GameConfig.hpp"
#include "Player.hpp"

#include "ResourceKeys.hpp"

#include "raylib.h"
#include "raymath.h"


void EnemyManager::Init(Player *player)
{
	_player = player;

	_defs = {
		{
			{ RK::COCKROACH_MOVE, 64, 64, 8, 8.0f, 90.0f, true },
			{ RK::COCKROACH_DEATH, 64, 64, 32, 64.0f, 90.0f, false },
			1.0f, 80.0f, 30.0f, 1.0f, 2.0f
		},
		{
			{ RK::SCORPION_MOVE, 64, 64, 4, 4.0f, 90.0f, true },
			{ RK::SCORPION_DEATH, 64, 64, 8, 16.0f, 90.0f, false },
			1.0f, 50.0f, 30.0f, 1.7f, 4.0f
		},
		{
			{ RK::KLIVER_MOVE, 64, 64, 8, 8.0f, 90.0f, true },
			{ RK::KLIVER_DEATH, 64, 64, 16, 32.0f, 90.0f, false },
			1.0f, 100.0f, 30.0f, 2.0f, 3.0f
		}
	};
}

Vector2 EnemyManager::pickSpawnPoint() const
{
	return RandomSpawnPoint(_player->GetPosition(), 200.0f);
}

void EnemyManager::SpawnBatch(int count)
{
	_batchTotal     = count;
	_batchRemaining = count;
	_staggerTimer   = 0.0f;
}

void EnemyManager::CancelBatch()
{
	_batchRemaining = 0;
	_batchTotal     = 0;
	_staggerTimer   = 0.0f;
}

void EnemyManager::Spawn(const EnemyDef& def, Vector2 pos)
{
	auto* enemy = spawnInPool();
	enemy->Init(def);
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

			int typeIndex = GetRandomValue(0, (int)_defs.size() - 1);

			Spawn(_defs[typeIndex], pickSpawnPoint());
		}
	}

	PoolManager<Enemy>::Update(dt);
}