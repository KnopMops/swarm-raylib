#pragma once

#include "Enemy.hpp"
#include "PoolManager.hpp"

#include "vector"


class Player;

class EnemyManager : public PoolManager<Enemy>
{
public:
	void Init(Player *player);

	void SpawnBatch(int count);
	void CancelBatch();

	void Update(float dt) override;

	bool IsBatchComplete() const { return _batchRemaining == 0 && CountAlive() == 0; };

	int GetBatchTotal()     const { return _batchTotal; }
	int GetBatchRemaining() const { return _batchRemaining; }

private:
	void Spawn(const EnemyDef& def, Vector2 pos);
	Vector2 pickSpawnPoint() const;

	std::vector<EnemyDef> _defs;

	Player* _player = nullptr;
	float _staggerInterval = 0.15f;
	float _staggerTimer    = 0.0f;
	int   _batchRemaining  = 0;
	int   _batchTotal      = 0;
};