#pragma once

#include "Enemy.hpp"
#include "PoolManager.hpp"


class Player;

class EnemyManager : public PoolManager<Enemy>
{
public:
	void Init(Player *player);

	void SpawnBatch(int count);
	void CancelBatch();

	void Update(float dt) override;

	bool IsBatchComplete() const { return _batchRemaining == 0 && CountAlive() == 0; };

	// Сколько врагов заказано на текущую волну (не уменьшается по мере спавна).
	int GetBatchTotal()     const { return _batchTotal; }
	// Сколько ещё предстоит выпустить.
	int GetBatchRemaining() const { return _batchRemaining; }

private:
	void Spawn(Vector2 pos);
	Vector2 pickSpawnPoint() const;

	Player* _player = nullptr;

	float _staggerInterval = 0.15f;
	float _staggerTimer    = 0.0f;
	int   _batchRemaining  = 0;
	int   _batchTotal      = 0;
};