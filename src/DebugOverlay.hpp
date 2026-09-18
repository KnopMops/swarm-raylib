#pragma once

#include "raylib.h"


class Player;
class BulletManager;
class EnemyManager;

class DebugOverlay
{
public:
	void Init(const Player& player, const BulletManager& bulletManager, const EnemyManager& enemyManager, const Camera2D& camera);
	void Draw(const Font& font) const;

private:
	const Player* _player = nullptr;
	const BulletManager* _bulletManager = nullptr;
	const EnemyManager* _enemyManager = nullptr;
	const Camera2D* _camera = nullptr;
};