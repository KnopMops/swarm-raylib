#pragma once

#include "raylib.h"

#include "Player.hpp"
#include "CollisionMap.hpp"

#include "BulletManager.hpp"
#include "EnemyManager.hpp"
#include "HealthPotionManager.hpp"

#include "Minimap.hpp"


enum class GameState { Playing, GameOver };

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
	void drawHud(const Font& font);

	void drawGameOverOverlay(const Font& font);

	void updateEntities(float dt);
	void updateCamera();
	void updateShooting();

	void startWave(int n);
	void spawnWaveEnemies();
	void updateWaves(float dt);

	void restart();

	void updateCollisions();

	Player _player;
	CollisionMap _collisionMap;
	Camera2D _camera = {};

	BulletManager _bullets;
	EnemyManager _enemies;
	HealthPotionManager _healthPotions;

	Minimap _minimap;

	int _wave = 1;
	float _waveTime = 0.0f;
	bool _wavePaused = false;
	bool _waveStarting = false;
	float _pauseTimer = 0.0f;

	GameState _gameState = GameState::Playing;
};