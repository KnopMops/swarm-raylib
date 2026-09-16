#include "EnemyManager.hpp"
#include "GameConfig.hpp"

#include "raylib.h"


void EnemyManager::Init(Player *player)
{
	_player = player;
}

void EnemyManager::SpawnBatch(int count)
{
	_batchRemaining = count;
	_staggerTimer = 0.0f;

	TraceLog(LOG_INFO, "ENEMY_MGR: BATCH of %d enemies queded", count);
}

void EnemyManager::Spawn(Vector2 pos)
{
	for (auto& enemy : _pool)
	{
		if (!enemy->IsAlive())
		{
			enemy->Activate(pos);
			return;
		}
	}

	auto enemy = std::make_unique<Enemy>();
	enemy->Activate(pos);
	enemy->SetPlayer(_player);
	_pool.push_back(std::move(enemy));
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
			Spawn({
				RandomFloat(0.0f, GameConfig::MAP_W),
				RandomFloat(0.0f, GameConfig::MAP_H)
			});
		}
	}

	for (const auto& e : _pool) e->Update(dt);
}

void EnemyManager::Draw()
{
	for (const auto& e : _pool) e->Draw();
}

void EnemyManager::DeactivateAll()
{
	for (const auto& e : _pool)
		if (e->IsAlive()) e->Deactivate();
}