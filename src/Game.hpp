#pragma once

#include "raylib.h"

#include "Player.hpp"
#include "CollisionMap.hpp"

#include "BulletManager.hpp"
#include "EnemyManager.hpp"
#include "HealthPotionManager.hpp"
#include "BookManager.hpp"

#include "Minimap.hpp"
#include "DebugOverlay.hpp"


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
	void drawHud(const Font& font);

	void drawGameOverOverlay(const Font& font);

	void updateEntities(float dt);
	void updateCamera();
	void updateShooting();
	void updateHealthBlink(float dt);

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
	BookManager _books;

	Minimap _minimap;
	DebugOverlay _debugOverlay;

	int _wave = 1;
	float _waveTime = 0.0f;
	bool _wavePaused = false;
	bool _waveStarting = false;
	float _pauseTimer = 0.0f;

	const Texture2D* _lifeTex = nullptr;
	Rectangle _lifeSrc = {};

	std::vector<Rectangle> _lifeDest;

	int   _lastHealth      = 0;
	int   _blinkPrevHealth = 0;
	float _blinkTimer      = 0.0f;

	static constexpr float HEALTH_BLINK_DURATION = 0.45f;

	GameState _gameState = GameState::Playing;
};