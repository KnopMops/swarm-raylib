#include "EnemyManager.hpp"
#include "raylib.h"


void EnemyManager::Init(Player *player)
{
	_player = player;
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