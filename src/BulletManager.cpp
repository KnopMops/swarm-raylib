#include "BulletManager.hpp"

void BulletManager::Spawn(Vector2 pos, float angleDeg)
{
	for (auto& bullet : _pool)
	{
		if (!bullet->IsAlive())
		{
			bullet->Activate(pos, angleDeg, _bulletSpeed);
			return;
		}
	}

	auto bullet = std::make_unique<Bullet>();
	bullet->Activate(pos, angleDeg, _bulletSpeed);
	_pool.push_back(std::move(bullet));
}

void BulletManager::Update(float dt)
{
	for (const auto& b : _pool) b->Update(dt);
}

void BulletManager::Draw()
{
	for (const auto& b : _pool) b->Draw();
}