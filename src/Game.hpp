#pragma once

#include "raylib.h"

#include "Player.hpp"
#include "CollisionMap.hpp"

#include "BulletManager.hpp"
#include "EnemyManager.hpp"


class Game
{
public:
	Game();
	~Game();

	bool HandleInput();

	void Update(float dt);
	void Draw(RenderTexture2D& canvas);

private:
	void drawWorld();
	void drawDebug(const Font& font);

	void updateEntities(float dt);
	void updateCamera();
	void updateShooting();

	void startWave(int n);
	void updateWaves();

	void updateCollisions();

	Player _player;
	CollisionMap _collisionMap;
	Camera2D _camera = {};

	BulletManager _bullets;
	EnemyManager _enemies;

	int _wave = 1;
};